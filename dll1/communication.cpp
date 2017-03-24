#define UMDF_USING_NTSTATUS
#include <windows.h>
#include <FltUser.h>
#include <ntstatus.h>

#include <memory>
#include <thread>
#include <vector>

#include "user_kernel_structs.h"
#include "dll1_public.h"

using namespace std;

static HANDLE gDllConnLock = NULL;
volatile HANDLE gDllConnFilterPort = NULL;
DWORD gDllConnThreadCount = 0;
HANDLE gRequestTerminateDllConnThreads = NULL;
void *gCallbackContext = NULL;

#define MESSAGE_BUFFER_SIZE                 65536 + 1024

typedef struct _THREAD_CONTEXT
{
    HANDLE Thread;
    DWORD Index;
    OVERLAPPED Overlapped;
    BYTE MessageBuffer[MESSAGE_BUFFER_SIZE];
}THREAD_CONTEXT, *PTHREAD_CONTEXT;

vector<thread> gCommunicationThreads;


void
UmCommunicationThreadBody(
    _In_ shared_ptr<THREAD_CONTEXT> ThreadContext
)
{
    HRESULT result, termError = 0;
    bool terminatedBecauseError = false;

    LogPrint("UmCommunicationThreadBody started \n");

    UNREFERENCED_PARAMETER(ThreadContext);

#pragma warning(suppress:4127)
    while (TRUE)
    {
        // test to see if terminate is requested
        if (WAIT_OBJECT_0 == WaitForSingleObject(gRequestTerminateDllConnThreads, 0))
        {
            terminatedBecauseError = false;
            LogPrint("dllconn thread %d terminated, because gRequestTerminateDllConnThreads is set\n", GetCurrentThreadId());
            break;
        }

        result = FilterGetMessage(
            gDllConnFilterPort,
            (PFILTER_MESSAGE_HEADER)&ThreadContext->MessageBuffer,
            MESSAGE_BUFFER_SIZE,
            &ThreadContext->Overlapped);

        if (result == HRESULT_FROM_WIN32(ERROR_IO_PENDING))
        {
            HANDLE waitHandles[2];
            DWORD waitResult;
            DWORD bytesTransferred;

            // pending stuff
            waitHandles[0] = gRequestTerminateDllConnThreads;
            waitHandles[1] = ThreadContext->Overlapped.hEvent;

            waitResult = WaitForMultipleObjects(
                2,
                waitHandles,
                FALSE,
                INFINITE);

            if (WAIT_OBJECT_0 == waitResult)
            {
                terminatedBecauseError = FALSE;
                LogPrint("dllconn thread %d terminated, because gRequestTerminateDllConnThreads is set\n", GetCurrentThreadId());
                break;
            }

            if ((WAIT_OBJECT_0 + 1) != waitResult)
            {
                int errCode = GetLastError();

                LogPrint("dllconn thread %d terminated, because error on WaitForMultipleObjects, wait result 0x%08x, err code 0x%08x\n",
                    GetCurrentThreadId(), waitResult, errCode);
                termError = errCode;
                break;
            }

            //
            // overlapped event signalled ==> check the operation result
            //
            if (!GetOverlappedResult(
                gDllConnFilterPort,
                &ThreadContext->Overlapped,
                &bytesTransferred,
                FALSE))
            {
                int errCode = GetLastError();

                if (ERROR_INVALID_HANDLE == errCode)
                {
                    LogPrint( "dllconn port disconnected!\n");
                    terminatedBecauseError = false;
                    break;
                }

                LogPrint( "dllconn thread %d terminated, because error on WaitForMultipleObjects, wait result 0x%08x, err code 0x%08x\n",
                    GetCurrentThreadId(), waitResult, errCode);
                termError = errCode;
                break;
            }
        }
        else if (result == HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE))
        {
            LogPrint("dllconn port disconnected!\n");
            terminatedBecauseError = false;
            break;
        }
        else if (result == HRESULT_FROM_WIN32(ERROR_OPERATION_ABORTED))     // 0x800703E3
        {
            LogPrint("operation aborted because of thread exit / app request ==> terminate scanner thread!\n");
            terminatedBecauseError = false;
            break;
        }
        else if (result != HRESULT_FROM_WIN32(S_OK))
        {
            LogPrint( "invalid result for FilterGetMessage if thread %d, error code 0x%08x ==> terminate thread",
                GetCurrentThreadId(), result);
            termError = result;
            break;
        }
        PCOMMAND_REQUEST pRequest = (PCOMMAND_REQUEST)&ThreadContext->MessageBuffer;

        switch (pRequest->Command)
        {

        case cmdGetLibraryVersion:
        {
            VERSION_INFORMATION_REPLY reply;

            // build and send reply
            reply.ReplyHeader.MessageId = pRequest->MessageHeader.MessageId;
            reply.ReplyHeader.Status = STATUS_SUCCESS;
            reply.VersionInformation.Command = cmdGetLibraryVersion;

            reply.VersionInformation.Major = 0;
            reply.VersionInformation.Minor = 0;
            reply.VersionInformation.Revision = 0;
            reply.VersionInformation.Build = 0;

            result = FilterReplyMessage(
                gDllConnFilterPort,
                &reply.ReplyHeader,
                sizeof(FILTER_REPLY_HEADER) + sizeof(VERSION_INFORMATION));

            if (S_OK != result)
            {
                LogPrint("ERROR: invalid result code for FilterReplyMessage 0x%08x \n",
                    result);
            }
        }
        break;

        default:
            LogPrint("ERROR: unrecognized message in dllconn thread (command code %d) \n", pRequest->Command);
            break;
        } // switch (pRequest->CommandCode)

        continue;
    }

    if (terminatedBecauseError)
    {
        LogPrint( "STOP-ON-ERROR: DllConnThreadBody, index %d inside PID 0x%08x, termError 0x%08x\n",
            ThreadContext->Index, GetCurrentProcessId(), termError);
    }
    else
    {
        LogPrint("STOP: DllConnThreadBody, index %d inside PID 0x%08x\n",
            ThreadContext->Index, GetCurrentProcessId());
    }
}

void
InitializeCommunication(
    __int32 NrOfDllConnThreads
)
{
    HANDLE dllConnFilterPort;
    HRESULT result;
    __int32 i;

    if (NrOfDllConnThreads == 0)
    {
        throw STATUS_INVALID_PARAMETER;
    }

    gRequestTerminateDllConnThreads = CreateEvent(NULL, TRUE, FALSE, NULL);


    result = FilterConnectCommunicationPort(
        DRIVER_PORT,
        0,
        NULL,  
        0,     
        NULL,
        &dllConnFilterPort);
    if (!SUCCEEDED(result))
    {
        LogPrint( "could not connect to filter, result 0x%08x\n", result);
        throw STATUS_CONNECTION_REFUSED;
    }

    gDllConnFilterPort = dllConnFilterPort;

    gDllConnThreadCount = 0;
    for (i = 0; i < NrOfDllConnThreads; i++)
    {
        shared_ptr<THREAD_CONTEXT> threadContext = make_shared<THREAD_CONTEXT>();

        threadContext->Thread = NULL;
        threadContext->Index = i;

        memset(&threadContext->Overlapped, 0, sizeof(OVERLAPPED));
        threadContext->Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        thread communication_thread(UmCommunicationThreadBody, threadContext);
        gCommunicationThreads.push_back(std::move(communication_thread));

    }

}

void
UninitializeCommunication()
{
    if (gCommunicationThreads.size() == 0)
    {
        return;
    }

    if ((NULL == gDllConnFilterPort) || (INVALID_HANDLE_VALUE == gDllConnFilterPort))
    {
        gDllConnFilterPort = NULL;
        return;
    }

    SetEvent(gRequestTerminateDllConnThreads);
    for (auto it = gCommunicationThreads.begin(); it != gCommunicationThreads.end(); ++it)
    {
        it->join();
    }
    gCommunicationThreads.clear();
    gDllConnThreadCount = 0;

    CloseHandle(gRequestTerminateDllConnThreads);
    gRequestTerminateDllConnThreads = NULL;

    CloseHandle(gDllConnFilterPort);
    gDllConnFilterPort = NULL;

}

__int32
TestCommand()
{
    NTSTATUS status;
    CMD_TEST_WITH_REPLY structCmdTest;
    DWORD bytesReturned;
    HRESULT result;

    structCmdTest.Command = cmdTestWithReply;
    status = STATUS_UNSUCCESSFUL;

    // are we connected to the driver?
    if (NULL == gDllConnFilterPort)
    {
        LogPrint("ERROR: driver1 NOT running\n");
        return STATUS_NOT_FOUND;
    }

    // effectively send message to the driver
    result = FilterSendMessage(
        gDllConnFilterPort,
        &structCmdTest,
        sizeof(CMD_TEST_WITH_REPLY),
        &structCmdTest,
        sizeof(CMD_TEST_WITH_REPLY),
        &bytesReturned);

    if (S_OK == result) 
    {
       LogPrint("Got %x from driver \n", structCmdTest.ReplyFromKernel);
       status = STATUS_SUCCESS;
    }
    else
    {
        LogPrint("ERROR: invalid result code for FilterSendMessage 0x%08x\n", result);

        status = STATUS_UNSUCCESSFUL;
    }

    return status;
}

__int32
ToggleProcessMonitoring()
{
	__debugbreak();
	NTSTATUS status;
	CMD_PROCMON_WITH_REPLY structProcmon;
	DWORD bytesReturned;
	HRESULT result;

	structProcmon.Command = cmdToggleProcmon;
	status = STATUS_UNSUCCESSFUL;

	// Check connectivity to driver:
	if (NULL == gDllConnFilterPort)
	{
		LogPrint("ERROR: driver1 NOT running\n");
		return STATUS_NOT_FOUND;
	}

	// effectively send message to the driver
	result = FilterSendMessage(
		gDllConnFilterPort,
		&structProcmon,
		sizeof(CMD_TEST_WITH_REPLY),
		&structProcmon,
		sizeof(CMD_TEST_WITH_REPLY),
		&bytesReturned);

	if (S_OK == result)
	{
		LogPrint("Got %x from driver \n", structProcmon.ReplyFromKernel);
		status = STATUS_SUCCESS;
	}
	else
	{
		LogPrint("ERROR: invalid result code for FilterSendMessage 0x%08x\n", result);

		status = STATUS_UNSUCCESSFUL;
	}
	return status;
}


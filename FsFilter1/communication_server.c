#include "driver1.h"
#include "user_kernel_structs.h"
#include "communication_server.h"

extern DRV_GLOBAL_DATA gDrv;

void
GetAndLogClientVersion();

void
GetAndLogProcDetails(
	WCHAR name[]
);

NTSTATUS
ClientReceiveMessage(
    _In_opt_ PVOID PortCookie,
    _In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_to_opt_(OutputBufferLength, *ReturnOutputBufferLength) PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength,
    _Out_ PULONG ReturnOutputBufferLength
);


NTSTATUS
ClientConnectedCallback(
    _In_ PFLT_PORT ClientPort,
    _In_opt_ PVOID ServerPortCookie,
    _In_reads_bytes_opt_(SizeOfContext) PVOID ConnectionContext,
    _In_ ULONG SizeOfContext,
    _Outptr_result_maybenull_ PVOID *ConnectionPortCookie
);

VOID
ClientDisconnectCallback(
    _In_opt_ PVOID ConnectionCookie
);

NTSTATUS
InitializeCommunication()
{
    NTSTATUS status;
    UNICODE_STRING uniStr;
    OBJECT_ATTRIBUTES objAttr;
    PSECURITY_DESCRIPTOR secDesc;

    status = STATUS_UNSUCCESSFUL;
    secDesc = NULL;

    if (NULL != gDrv.DllConnServerPort)
    {
        LOG("WARNING: DrvInitializeCommPort called, but already initialized\n");
        return STATUS_OBJECT_NAME_EXISTS;
    }

    RtlInitUnicodeString(&uniStr, DRIVER_PORT);

    // we secure the port so only ADMINs & SYSTEM can access it
	LOG("ABOUT TO CALL:::: FltBuildDefaultSecurityDescriptor");
    status = FltBuildDefaultSecurityDescriptor(&secDesc, FLT_PORT_ALL_ACCESS);
    if (!NT_SUCCESS(status))
    {
        LOG("CRITICAL: FltBuildDefaultSecurityDescriptor failed, status=0x%x \n",
            status);
        goto cleanup;
    }

    InitializeObjectAttributes(
        &objAttr,
        &uniStr,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL,
        secDesc);

	LOG("ABOUT TO CALL:::: FltCreateCommunicationPort");
	//__debugbreak();
    status = FltCreateCommunicationPort(
        gDrv.FilterHandle,
        &gDrv.DllConnServerPort,
        &objAttr,
        NULL,
        ClientConnectedCallback,
        ClientDisconnectCallback,
        ClientReceiveMessage,
        1
    );
    if (!NT_SUCCESS(status))
    {
        LOG("CRITICAL: FltCreateCommunicationPort / DllPort failed, status=0x%x\n",
            status);
        goto cleanup;
    }

    status = STATUS_SUCCESS;

cleanup:
    if (NULL != secDesc)
    {
        // free the security descriptor in all cases; it is not needed once the call to FltCreateCommunicationPort() is made
		LOG("ABOUT TO CALL:::: FltFreeSecurityDescriptor");
		FltFreeSecurityDescriptor(secDesc);
        secDesc = NULL;
    }

    return status;

}

NTSTATUS
ClientConnectedCallback(
    _In_ PFLT_PORT ClientPort,
    _In_opt_ PVOID ServerPortCookie,
    _In_reads_bytes_opt_(SizeOfContext) PVOID ConnectionContext,
    _In_ ULONG SizeOfContext,
    _Outptr_result_maybenull_ PVOID *ConnectionPortCookie
)
{
    UNREFERENCED_PARAMETER(ServerPortCookie);
    UNREFERENCED_PARAMETER(ConnectionContext);
    UNREFERENCED_PARAMETER(SizeOfContext);
    UNREFERENCED_PARAMETER(ConnectionPortCookie);

    gDrv.DllConnClientPort = ClientPort;

	
    return STATUS_SUCCESS;
}

VOID
ClientDisconnectCallback(
    _In_opt_ PVOID ConnectionCookie
)
{
    LOG("Client disconnected \n");

    UNREFERENCED_PARAMETER(ConnectionCookie);

	LOG("ABOUT TO CALL:::: FltCloseClientPort");
    FltCloseClientPort(gDrv.FilterHandle, (PFLT_PORT*)&gDrv.DllConnClientPort);
    gDrv.DllConnClientPort = NULL;
}
NTSTATUS
ClientReceiveMessage(
    _In_opt_ PVOID PortCookie,
    _In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_to_opt_(OutputBufferLength, *ReturnOutputBufferLength) PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength,
    _Out_ PULONG ReturnOutputBufferLength
)
{
	//__debugbreak();
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    UNREFERENCED_PARAMETER(PortCookie);
    COMMAND_CODE cmdCode;

    __try
    {
        cmdCode = *((PCOMMAND_CODE)InputBuffer);
        switch (cmdCode)
        {

       case cmdTestWithReply:
        {
            PCMD_TEST_WITH_REPLY cmd = (PCMD_TEST_WITH_REPLY)InputBuffer;
            PCMD_TEST_WITH_REPLY cmdReply = (PCMD_TEST_WITH_REPLY)OutputBuffer;

            cmd; 

            if ((NULL == OutputBuffer) || (OutputBufferLength < sizeof(CMD_TEST_WITH_REPLY)) ||
                (InputBufferLength < sizeof(CMD_TEST_WITH_REPLY)))
            {
                LOG("status invalid parameter for received message type 0x%x \n", cmdCode);
                status = STATUS_INVALID_PARAMETER;
                break;
            }

            cmdReply->ReplyFromKernel = 0xDeadBeef;
            status = STATUS_SUCCESS;
            
            *ReturnOutputBufferLength = sizeof(CMD_TEST_WITH_REPLY);

            GetAndLogClientVersion();
            
        }break;

		// TODO
	   case cmdEnableProcmon:
	   {
		   PCMD_PROCMON_WITH_REPLY cmd = (PCMD_PROCMON_WITH_REPLY)InputBuffer;
		   PCMD_PROCMON_WITH_REPLY cmdReply = (PCMD_PROCMON_WITH_REPLY)OutputBuffer;

		   (void)cmd;

		   if ((NULL == OutputBuffer) || (OutputBufferLength < sizeof(CMD_PROCMON_WITH_REPLY)) ||
			   (InputBufferLength < sizeof(CMD_PROCMON_WITH_REPLY)))
		   {
			   LOG("status invalid parameter for received message type 0x%x \n", cmdCode);
			   status = STATUS_INVALID_PARAMETER;
			   break;
		   }

		   DrvInitializeProcNotify();

		   cmdReply->ReplyFromKernel = 0xB00BB00B;
		   status = STATUS_SUCCESS;

		   *ReturnOutputBufferLength = sizeof(CMD_PROCMON_WITH_REPLY);
	   }break;

	   // TODO
	   case cmdDisableProcmon:
	   {
		   PCMD_PROCMON_WITH_REPLY cmd = (PCMD_PROCMON_WITH_REPLY)InputBuffer;
		   PCMD_PROCMON_WITH_REPLY cmdReply = (PCMD_PROCMON_WITH_REPLY)OutputBuffer;

		   (void)cmd;

		   if ((NULL == OutputBuffer) || (OutputBufferLength < sizeof(CMD_PROCMON_WITH_REPLY)) ||
			   (InputBufferLength < sizeof(CMD_PROCMON_WITH_REPLY)))
		   {
			   LOG("status invalid parameter for received message type 0x%x \n", cmdCode);
			   status = STATUS_INVALID_PARAMETER;
			   break;
		   }

		   DrvUninitializeProcNotify();

		   cmdReply->ReplyFromKernel = 0xDEADDEAD;
		   status = STATUS_SUCCESS;

		   *ReturnOutputBufferLength = sizeof(CMD_PROCMON_WITH_REPLY);
	   }break;
	   
	   /*
	   case cmdGiveProcname:
	   {
		 //  PCMD_PROCMON_WITH_REPLY cmd = (PCMD_TEST_WITH_REPLY)InputBuffer;
		   PCMD_TEST_WITH_REPLY cmdReply = (PCMD_TEST_WITH_REPLY)OutputBuffer;
		   
		 //  cmd;

		   if ((NULL == OutputBuffer) || (OutputBufferLength < sizeof(CMD_TEST_WITH_REPLY)) ||
			   (InputBufferLength < sizeof(CMD_TEST_WITH_REPLY)))
		   {
			   LOG("status invalid parameter for received message type 0x%x \n", cmdCode);
			   status = STATUS_INVALID_PARAMETER;
			   break;
		   }

		   cmdReply->ReplyFromKernel = 0xDeadBeef;
		   status = STATUS_SUCCESS;

		   *ReturnOutputBufferLength = sizeof(CMD_TEST_WITH_REPLY);

		   GetAndLogProcDetails("");
	   }break;   */

        default:
            LOG("ERROR: unrecognized command code %d\n", cmdCode);
            status = STATUS_INVALID_PARAMETER;
            break;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        LOG("EXCEPTION: code 0x%08x\n", status);
        return status;
    }

    return status;
}

void
GetAndLogClientVersion(
)
{
    VERSION_INFORMATION clientVersionInformation;
    ULONG replyLength = sizeof(VERSION_INFORMATION);

    RtlZeroMemory(&clientVersionInformation, sizeof(VERSION_INFORMATION));
    clientVersionInformation.Command = cmdGetLibraryVersion;

	LOG("ABOUT TO CALL:::: FltSendMessage");
    NTSTATUS status = FltSendMessage(gDrv.FilterHandle, (PFLT_PORT*)&gDrv.DllConnClientPort, &clientVersionInformation, sizeof(VERSION_INFORMATION),
        &clientVersionInformation, &replyLength, NULL);
    if (!NT_SUCCESS(status))
    {
        LOG_ERROR("FltSendMessage failed with status 0x%x \n", status);
    }
    else
    {
        LOG("	:: ~ :: ~ :: ~ :: ~ got version from UM client: Major %d Minor %d Revision %d Build %d \n", clientVersionInformation.Major, clientVersionInformation.Minor,
            clientVersionInformation.Revision, clientVersionInformation.Build);
    }
}

void
GetAndLogProcDetails(
	WCHAR name[]
)
{
	
	//__debugbreak();

	PROC_INFO procInfo;
	ULONG replyLength = sizeof(PROC_INFO);// sizeof(FILTER_REPLY_HEADER) + sizeof(PROC_INFO);
	
	RtlZeroMemory(&procInfo, sizeof(PPROC_INFO));
	procInfo.Command = cmdGiveProcname;

	// Transfer the name:
	memcpy(&procInfo.ImageFileName, name, sizeof(procInfo.ImageFileName));

	NTSTATUS status = FltSendMessage(gDrv.FilterHandle, (PFLT_PORT*)&gDrv.DllConnClientPort, &procInfo, sizeof(PROC_INFO),
		&procInfo, &replyLength, NULL);
		
	if (!NT_SUCCESS(status))
	{
		LOG_ERROR("\n\n*******8888*********8888888888***********FltSendMessage failed with status 0x%x \n\n\n", status);
	}
	else
	{
		LOG("\n\nESTE OK MAI\n\n");
	}
	
}

void
UninitializeCommunication()
{
    if (NULL != gDrv.DllConnServerPort)
    {
		LOG("ABOUT TO CALL:::: FltCloseCommunicationPort");
        FltCloseCommunicationPort(gDrv.DllConnServerPort);
        gDrv.DllConnServerPort = NULL;
    }

    PFLT_PORT clientPort = (PFLT_PORT)InterlockedExchangePointer((volatile PVOID*)&gDrv.DllConnClientPort, NULL);
    if (NULL != clientPort)
    {
		LOG("ABOUT TO CALL:::: FltCloseClientPort");
        FltCloseClientPort(gDrv.FilterHandle, &clientPort);
    }
}

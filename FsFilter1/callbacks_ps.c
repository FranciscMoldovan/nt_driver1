#include "driver1.h"
#include <Ntddk.h>
//
// global data needed for PS callbacks
//
BOOLEAN gProcessNotifyRegistered = FALSE;


void
DrvProcessNotifyRoutineCommon(
    __in_opt HANDLE ParentId,
    __in HANDLE ProcessId,
    __in BOOLEAN Create,
    __in_opt PPS_CREATE_NOTIFY_INFO CreateInfo );

void
DrvProcessNotifyRoutineEx(
    __inout PEPROCESS Process,
    __in HANDLE ProcessId,
    __in_opt PPS_CREATE_NOTIFY_INFO CreateInfo );



//
// DrvInitializeProcNotify
//
NTSTATUS
DrvInitializeProcNotify(
    void )
{
    NTSTATUS status;

    status = STATUS_UNSUCCESSFUL;

    if (FALSE != gProcessNotifyRegistered)
    {
        LOG("WARNING: DrvInitializeProcNotify called, but already initialized\n");
        return STATUS_OBJECT_NAME_EXISTS;          
    }
	//__debugbreak();
	LOG("::::::::::C A L L I N G  T H A T  S P E C I A L  F U N C T I O N!!!");
    status = PsSetCreateProcessNotifyRoutineEx(
                    DrvProcessNotifyRoutineEx,
                    FALSE );
  
    if (NT_SUCCESS(status))
    {
		
        LOG("DrvProcessNotifyRoutine successfully registered\n");
        gProcessNotifyRegistered = TRUE;
    }
    else
    {
        LOG("ERROR: PsSetCreateProcessNotifyRoutine/Ex failed, status=%s\n", status);
    }

    return status;
}


//
// DrvUninitializeProcNotify
//
NTSTATUS
DrvUninitializeProcNotify(
    void )
{
    NTSTATUS status;

    status = STATUS_UNSUCCESSFUL;

    if (TRUE == gProcessNotifyRegistered)
    {
      
		status = PsSetCreateProcessNotifyRoutineEx(
			DrvProcessNotifyRoutineEx,
			TRUE);
        gProcessNotifyRegistered = FALSE;

        if (!NT_SUCCESS(status))
        {
            LOG("ERROR: PsSetCreateProcessNotifyRoutine / remove failed, status=%x\n", status);
        }
    }
    else
    {
        status = STATUS_SUCCESS;
    }

    return status;
}


extern void
GetAndLogClientVersion();

extern void
LogProcessInfo(
	char name[]
);

void
DrvProcessNotifyRoutineCommon(
    __in_opt HANDLE ParentId,
    __in HANDLE ProcessId,
    __in BOOLEAN Create,
    __in_opt PPS_CREATE_NOTIFY_INFO CreateInfo
    )
{
	//__debugbreak();
    NTSTATUS status;

    status = STATUS_UNSUCCESSFUL;

    if (Create)
    {

		KdPrint(("INFO: Create process PID 0x%08Ix / %Id - '%wZ', parent 0x%08Ix / %Id, current TID 0x%08Ix\n",
			ProcessId, ProcessId, CreateInfo->ImageFileName,
			ParentId, ParentId,
			PsGetCurrentThreadId()));
    }
	///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   
	LOG("--><-o-><-o-><-o-><-o-><-o-><-o-><-o-><-o->\n");
	char test[5] = "test";
		(void)test;
	LogProcessInfo(test);
	LOG("--><-o-><-o-><-o-><-o-><-o-><-o-><-o-><-o->\n");
	///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///   

    if (!Create)
    {
        KdPrint(("INFO: terminate process with PID 0x%08Ix / %Id, current TID 0x%08Ix\n",
            ProcessId, ProcessId, PsGetCurrentThreadId() ));
    }
}

//
// DrvProcessNotifyRoutineEx
//
void
DrvProcessNotifyRoutineEx(
    __inout PEPROCESS Process,
    __in HANDLE ProcessId,
    __in_opt PPS_CREATE_NOTIFY_INFO CreateInfo
    )
{
	//__debugbreak();
    UNREFERENCED_PARAMETER(Process);

    if (NULL != CreateInfo)
    {
        DrvProcessNotifyRoutineCommon(CreateInfo->ParentProcessId, ProcessId, TRUE, CreateInfo);
    }
    else
    {
        DrvProcessNotifyRoutineCommon(NULL, ProcessId, FALSE, NULL);
    }
}

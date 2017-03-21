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

void
DrvProcessNotifyRoutineCommon(
    __in_opt HANDLE ParentId,
    __in HANDLE ProcessId,
    __in BOOLEAN Create,
    __in_opt PPS_CREATE_NOTIFY_INFO CreateInfo
    )
{
    NTSTATUS status;

    status = STATUS_UNSUCCESSFUL;

    if (Create)
    {

		KdPrint(("INFO: Create process PID 0x%08Ix / %Id - '%wZ', parent 0x%08Ix / %Id, current TID 0x%08Ix\n",
			ProcessId, ProcessId, CreateInfo->ImageFileName,
			ParentId, ParentId,
			PsGetCurrentThreadId()));
    }

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
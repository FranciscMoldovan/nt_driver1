#include "driver1.h"
#include "minifilter_callbacks.h"
#include "communication_server.h"

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")

extern DRV_GLOBAL_DATA gDrv;

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
);


NTSTATUS
DriverUnload(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
);


#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#endif

//
//  operation registration
//

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {


// TODO - List all of the requests to filter.
	{ IRP_MJ_CREATE,
    0,
    PreOperation,
    PostOperation },
#if 0
    { IRP_MJ_CREATE_NAMED_PIPE,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_CLOSE,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_READ,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_WRITE,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_QUERY_INFORMATION,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_SET_INFORMATION,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_QUERY_EA,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_SET_EA,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_FLUSH_BUFFERS,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_QUERY_VOLUME_INFORMATION,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_SET_VOLUME_INFORMATION,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_DIRECTORY_CONTROL,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_FILE_SYSTEM_CONTROL,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_DEVICE_CONTROL,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_INTERNAL_DEVICE_CONTROL,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_SHUTDOWN,
    0,
    PreOperationNoPostOperation,
    NULL },                               //post operations not supported

    { IRP_MJ_LOCK_CONTROL,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_CLEANUP,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_CREATE_MAILSLOT,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_QUERY_SECURITY,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_SET_SECURITY,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_QUERY_QUOTA,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_SET_QUOTA,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_PNP,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_ACQUIRE_FOR_MOD_WRITE,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_RELEASE_FOR_MOD_WRITE,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_ACQUIRE_FOR_CC_FLUSH,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_RELEASE_FOR_CC_FLUSH,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_NETWORK_QUERY_OPEN,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_MDL_READ,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_MDL_READ_COMPLETE,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_PREPARE_MDL_WRITE,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_MDL_WRITE_COMPLETE,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_VOLUME_MOUNT,
    0,
    PreOperation,
    PostOperation },

    { IRP_MJ_VOLUME_DISMOUNT,
    0,
    PreOperation,
    PostOperation },

#endif // TODO

    { IRP_MJ_OPERATION_END }
};


VOID
SectionContextCleanup(
	_In_ PFLT_CONTEXT Context,
	_In_ FLT_CONTEXT_TYPE ContextType
);

#define SECTION_CONTEXT_SIZE sizeof(SCAN_SECTION_CONTEXT)
#define SECTION_CONTEXT_TAG  'scTG'

FLT_CONTEXT_REGISTRATION ContextRegistration[] = {

	{ FLT_SECTION_CONTEXT,
	0,
	SectionContextCleanup,
	SECTION_CONTEXT_SIZE,
	SECTION_CONTEXT_TAG },

	{ FLT_CONTEXT_END }
};


VOID
SectionContextCleanup(
	_In_ PFLT_CONTEXT Context,
	_In_ FLT_CONTEXT_TYPE ContextType
)
{
	PAGED_CODE();

	UNREFERENCED_PARAMETER(Context);
	UNREFERENCED_PARAMETER(ContextType);
}

//
//  This defines what we want to filter with FltMgr
//

CONST FLT_REGISTRATION FilterRegistration = {

    sizeof(FLT_REGISTRATION),         //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  Flags

    ContextRegistration,                               //  Context
    Callbacks,                          //  Operation callbacks

    DriverUnload,                       //  MiniFilterUnload

    InstanceSetup,                    //  InstanceSetup
    InstanceQueryTeardown,            //  InstanceQueryTeardown
    InstanceTeardownStart,            //  InstanceTeardownStart
    InstanceTeardownComplete,         //  InstanceTeardownComplete

    NULL,                               //  GenerateFileName
    NULL,                               //  GenerateDestinationFileName
    NULL                                //  NormalizeNameComponent

};


DRV_GLOBAL_DATA gDrv;

void ProcessNotifyRoutine(
	_In_        HANDLE                 ParentId,
	_In_        HANDLE                 ProcessId,
	_Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
	NTSTATUS status = STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(ParentId);
	UNREFERENCED_PARAMETER(status);

	LOG("******* SetCreateProcessNotifyRoutineEx entry\n");

	if (CreateInfo != NULL)
	{
		LOG("ProcessId = %d\n", ProcessId);
		LOG("ImageFileName = %wZ\n", CreateInfo->ImageFileName);
		LOG("CommandLine = %wZ\n", CreateInfo->CommandLine);
	}

	LOG("******* SetCreateProcessNotifyRoutineEx exit \n");
}


// KEYS
LARGE_INTEGER g_CmCookie = { 0 };

NTSTATUS RfRegistryCallback(
	_In_ PVOID CallbackContext,
	_In_ PVOID Argument1,
	_In_ PVOID Argument2
);

void RfUnload(
	_In_ PDRIVER_OBJECT pDriverObject
);
// KEYS

/*************************************************************************
MiniFilter initialization and unload routines.
*************************************************************************/

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
/*++

Routine Description:

This is the initialization routine for this miniFilter driver.  This
registers with FltMgr and initializes all global data structures.

Arguments:

DriverObject - Pointer to driver object created by the system to
represent this driver.

RegistryPath - Unicode string identifying where the parameters for this
driver are located in the registry.

Return Value:

Routine can return non success error codes.

--*/
{
	//__debugbreak();
    NTSTATUS status;

    UNREFERENCED_PARAMETER(RegistryPath);
    RtlZeroMemory(&gDrv, sizeof(DRV_GLOBAL_DATA));
    LOG("DriverEntry: Entered\n");



    status = FltRegisterFilter(DriverObject,
        &FilterRegistration,
        &gDrv.FilterHandle);

	FLT_ASSERT(NT_SUCCESS(status));

	KeInitializeMutex(&gDrv.DllMessagingMutex, 0);
	KeInitializeMutex(&gDrv.ScanningMutex, 0);

	// Set up the unload routine
	DriverObject->DriverUnload = RfUnload;

	// Register our callback with the system
	UNICODE_STRING AltitudeString = RTL_CONSTANT_STRING(L"360000");

	status = CmRegisterCallbackEx(RfRegistryCallback, &AltitudeString, DriverObject, NULL, &g_CmCookie, NULL);
	if (!NT_SUCCESS(status))
	{
		LOG("FAILED TO REGISTER KEYS FILTER\n");
	}

    FLT_ASSERT(NT_SUCCESS(status));

    if (NT_SUCCESS(status))
    {

        status = FltStartFiltering(gDrv.FilterHandle);

        if (!NT_SUCCESS(status)) {

            LOG("[ERROR]: FltStartFiltering failed, status 0x%x \n", status);
            FltUnregisterFilter(gDrv.FilterHandle);
            return STATUS_UNSUCCESSFUL;
        }
    }

    status = InitializeCommunication();
    if (!NT_SUCCESS(status))
    {
        NT_ASSERT(0);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
DriverUnload(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(Flags);

    gDrv.ShuttingDown = TRUE;
    LOG("DriverUnload: Entered\n");

    UninitializeCommunication();
    FltUnregisterFilter(gDrv.FilterHandle);

    return STATUS_SUCCESS;
}

// KEYS
void RfUnload(_In_ PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	PAGED_CODE();
	NTSTATUS status = CmUnRegisterCallback(g_CmCookie);
	if (!NT_SUCCESS(status))
	{
		// Failed to unregister
	}
}

NTSTATUS RfRegistryCallback(
	_In_ PVOID CallbackContext,
	_In_ PVOID Argument1,
	_In_ PVOID Argument2
)
{
	PREG_DELETE_VALUE_KEY_INFORMATION PreDeleteValueInfo;

	UNREFERENCED_PARAMETER(CallbackContext);
	UNREFERENCED_PARAMETER(Argument2);
	REG_NOTIFY_CLASS Operation = (REG_NOTIFY_CLASS)(ULONG_PTR)Argument1;

	// KEYS
	LARGE_INTEGER localCookie = { 0 };

	PUNICODE_STRING objectName;

	switch (Operation)
	{
	case RegNtPreCreateKeyEx:
		break;
	case RegNtPreOpenKeyEx:
		break;
	case RegNtKeyHandleClose:
		break;
	case RegNtPreDeleteValueKey:
		PreDeleteValueInfo = (PREG_DELETE_VALUE_KEY_INFORMATION)Argument2;
		PreDeleteValueInfo;
		NTSTATUS status = CmCallbackGetKeyObjectID(&localCookie, PreDeleteValueInfo->Object, NULL, &objectName);
		status;
		LOG("********** => %ls\n",objectName->Buffer);
	default:
		break;
	}
	return STATUS_SUCCESS;
}
// KEYS

#include "driver1.h"
#include "minifilter_callbacks.h"


ULONG_PTR OperationStatusCtx = 1;


NTSTATUS
InstanceSetup(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(VolumeDeviceType);
    UNREFERENCED_PARAMETER(VolumeFilesystemType);

    PAGED_CODE();

    LOG("InstanceSetup: Entered\n");

    return STATUS_SUCCESS;
}


NTSTATUS
InstanceQueryTeardown(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
)
/*++

Routine Description:

This is called when an instance is being manually deleted by a
call to FltDetachVolume or FilterDetach thereby giving us a
chance to fail that detach request.

If this routine is not defined in the registration structure, explicit
detach requests via FltDetachVolume or FilterDetach will always be
failed.

Arguments:

FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
opaque handles to this filter, instance and its associated volume.

Flags - Indicating where this detach request came from.

Return Value:

Returns the status of this operation.

--*/
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    LOG("InstanceQueryTeardown: Entered\n");

    return STATUS_SUCCESS;
}


VOID
InstanceTeardownStart(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
)
/*++

Routine Description:

This routine is called at the start of instance teardown.

Arguments:

FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
opaque handles to this filter, instance and its associated volume.

Flags - Reason why this instance is being deleted.

Return Value:

None.

--*/
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    LOG("InstanceTeardownStart: Entered\n");
}


VOID
InstanceTeardownComplete(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
)
/*++

Routine Description:

This routine is called at the end of instance teardown.

Arguments:

FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
opaque handles to this filter, instance and its associated volume.

Flags - Reason why this instance is being deleted.

Return Value:

None.

--*/
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    LOG("InstanceTeardownComplete: Entered\n");
}

FLT_PREOP_CALLBACK_STATUS
PreOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
/*++

Routine Description:

This routine is a pre-operation dispatch routine for this miniFilter.

This is non-pageable because it could be called on the paging path

Arguments:

Data - Pointer to the filter callbackData that is passed to us.

FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
opaque handles to this filter, instance, its associated volume and
file object.

CompletionContext - The context for the completion routine for this
operation.

Return Value:

The return value is the status of the operation.

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

  //  LOG(" * * * PreOperation: Entered\n");
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(status);

	PFLT_FILE_NAME_INFORMATION fileNameInfo;
	status = FltGetFileNameInformation(
		//_In_  PFLT_CALLBACK_DATA      
		//CallbackData,
		Data,
		//_In_  FLT_FILE_NAME_OPTIONS      
		//NameOptions,
		FLT_FILE_NAME_NORMALIZED,
		//_Out_ PFLT_FILE_NAME_INFORMATION 
		&fileNameInfo
	);
	status;
	LOG(" #$ #$ #$ #$ #$ => %wZ\n", fileNameInfo->Name);

    //
    //  See if this is an operation we would like the operation status
    //  for.  If so request it.
    //
    //  NOTE: most filters do NOT need to do this.  You only need to make
    //        this call if, for example, you need to know if the oplock was
    //        actually granted.
    //

    if (DoRequestOperationStatus(Data)) {

        status = FltRequestOperationStatusCallback(Data,
            OperationStatusCallback,
            (PVOID)(++OperationStatusCtx)
        );
        if (!NT_SUCCESS(status)) {

            LOG("PreOperation: FltRequestOperationStatusCallback Failed, status=%08x\n",
                status);
        }
    }

    // This template code does not do anything with the callbackData, but
    // rather returns FLT_PREOP_SUCCESS_WITH_CALLBACK.
    // This passes the request down to the next miniFilter in the chain.

    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}



VOID
OperationStatusCallback(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
    _In_ NTSTATUS OperationStatus,
    _In_ PVOID RequesterContext
)
/*++

Routine Description:

This routine is called when the given operation returns from the call
to IoCallDriver.  This is useful for operations where STATUS_PENDING
means the operation was successfully queued.  This is useful for OpLocks
and directory change notification operations.

This callback is called in the context of the originating thread and will
never be called at DPC level.  The file object has been correctly
referenced so that you can access it.  It will be automatically
dereferenced upon return.

This is non-pageable because it could be called on the paging path

Arguments:

FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
opaque handles to this filter, instance, its associated volume and
file object.

RequesterContext - The context for the completion routine for this
operation.

OperationStatus -

Return Value:

The return value is the status of the operation.

--*/
{
    UNREFERENCED_PARAMETER(FltObjects);

    LOG("OperationStatusCallback: Entered\n");

    LOG("OperationStatusCallback: Status=%08x ctx=%p IrpMj=%02x.%02x \"%s\"\n",
        OperationStatus,
        RequesterContext,
        ParameterSnapshot->MajorFunction,
        ParameterSnapshot->MinorFunction,
        FltGetIrpName(ParameterSnapshot->MajorFunction));
}


FLT_POSTOP_CALLBACK_STATUS
PostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
)
/*++

Routine Description:

This routine is the post-operation completion routine for this
miniFilter.

This is non-pageable because it may be called at DPC level.

Arguments:

Data - Pointer to the filter callbackData that is passed to us.

FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
opaque handles to this filter, instance, its associated volume and
file object.

CompletionContext - The completion context set in the pre-operation routine.

Flags - Denotes whether the completion is successful or is being drained.

Return Value:

The return value is the status of the operation.

--*/
{
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);
    UNREFERENCED_PARAMETER(Flags);

   // LOG(" * * * PostOperation: Entered\n");

    return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS
PreOperationNoPostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
/*++

Routine Description:

This routine is a pre-operation dispatch routine for this miniFilter.

This is non-pageable because it could be called on the paging path

Arguments:

Data - Pointer to the filter callbackData that is passed to us.

FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
opaque handles to this filter, instance, its associated volume and
file object.

CompletionContext - The context for the completion routine for this
operation.

Return Value:

The return value is the status of the operation.

--*/
{
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    LOG("PreOperationNoPostOperation: Entered\n");

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

BOOLEAN
DoRequestOperationStatus(
    _In_ PFLT_CALLBACK_DATA Data
)


/*++

Routine Description:

This identifies those operations we want the operation status for.  These
are typically operations that return STATUS_PENDING as a normal completion
status.

Arguments:

Return Value:

TRUE - If we want the operation status
FALSE - If we don't

--*/
{
    PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;

    //
    //  return boolean state based on which operations we are interested in
    //

    return (BOOLEAN)

        //
        //  Check for oplock operations
        //

        (((iopb->MajorFunction == IRP_MJ_FILE_SYSTEM_CONTROL) &&
        ((iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_FILTER_OPLOCK) ||
            (iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_BATCH_OPLOCK) ||
            (iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_1) ||
            (iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_2)))

            ||

            //
            //    Check for directy change notification
            //

            ((iopb->MajorFunction == IRP_MJ_DIRECTORY_CONTROL) &&
            (iopb->MinorFunction == IRP_MN_NOTIFY_CHANGE_DIRECTORY))
            );
}

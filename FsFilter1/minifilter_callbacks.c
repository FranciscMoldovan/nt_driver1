#include "driver1.h"
#include "minifilter_callbacks.h"
#include "user_kernel_structs.h"


ULONG_PTR OperationStatusCtx = 1;

extern DRV_GLOBAL_DATA gDrv;

NTSTATUS
ScannerpScanFileInUserMode(
	_In_ PFLT_INSTANCE Instance,
	_In_ PFILE_OBJECT FileObject,
	_Out_ PBOOLEAN SafeToOpen
);

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
	//  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //
	NTSTATUS status;

    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

  //  LOG(" * * * PreOperation: Entered\n");
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(status);

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

	NTSTATUS status;

	// If this create was failing anyway, avoid scaning now
	if (!NT_SUCCESS(Data->IoStatus.Status) ||
		(STATUS_REPARSE == Data->IoStatus.Status))
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}

	// Check if it has the .txt extension
	PFLT_FILE_NAME_INFORMATION fileNameInfo;
	status = FltGetFileNameInformation(
		Data,
		FLT_FILE_NAME_OPENED |
		FLT_FILE_NAME_QUERY_FILESYSTEM_ONLY,
		&fileNameInfo
	);

	PWCH* p_str = &fileNameInfo->Name.Buffer;
	if (wcswcs(*p_str, L".txt") != NULL)
	{
		LOG("T X T file I'm working with %ls \n", *p_str);
		

	}
	p_str;
    
	// Release the file name info, as it is no longer needed
	FltReleaseFileNameInformation(fileNameInfo);

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

NTSTATUS
ScannerpScanFileInUserMode(
	_In_ PFLT_INSTANCE Instance,
	_In_ PFILE_OBJECT FileObject,
	_Out_ PBOOLEAN SafeToOpen
)
// Rountine sends a req to user mode to scan a given file and
// give a reply wether the contents is safe to open
// Should this scan fail, SafeToOpen is set to TRUE. Failure at scan
// may appear because the service hasn't started, or because tis create/
// cleanup is for a directory, and there os no data to read and scan
/*
Arguments:

Instance - Handle to the filter instance for the scanner on this volume.

FileObject - File to be scanned.

SafeToOpen - Set to FALSE if the file is scanned successfully and it contains
foul language.

Return Value :

The status of the operation, hopefully STATUS_SUCCESS.The common failure
status will probably be STATUS_INSUFFICIENT_RESOURCES.
*/
{
	NTSTATUS status = STATUS_SUCCESS;
//	PVOID buffer = NULL;
//	ULONG bytesRead;
	Instance; FileObject; SafeToOpen;
	*SafeToOpen = TRUE;
	PFLT_VOLUME volume = NULL;
	FLT_VOLUME_PROPERTIES volumeProps;
	ULONG replyLength, length, bytesRead;
	PVOID buffer = NULL;
	TEXTBUFF_INFO notification;
	LARGE_INTEGER offset;
	// Check for client port
//	if (gDrv.DllConnClientPort == NULL)
//	{
//		return STATUS_SUCCESS;
//	}
	replyLength;

	try
	{
		// Obtain the colume object
		status = FltGetVolumeFromInstance(Instance, &volume);
		if (!NT_SUCCESS(status))
		{
			leave;
		}
		
		// Get sector size. Noncached I/O can only be done at sector size offsets, and in lengths which are
		// multiples of sector size. A more efficient ways is making this call once and remeber the sector
		// size in the instance setup routine and setup an instance context where we can cache it

		status = FltGetVolumeProperties(volume,
										&volumeProps,
										sizeof(volumeProps),
										&length);
		// STATUS_BUFFER_OVERFLOW can be returned - however we only need the properties, not the names
		// hence we only check for error status
		if (NT_ERROR(status))
		{
			leave;
		}

		length = max(SCANNER_READ_BUFFER_SIZE, volumeProps.SectorSize);

		// Use non-buffered I/O, so allocate aligned pool
		buffer = FltAllocatePoolAlignedWithTag(Instance,
											   NonPagedPool,
											   length,
											   'nacS');
		if (NULL == buffer)
		{
			status = STATUS_INSUFFICIENT_RESOURCES; 
			leave;
		}

	//	notification = ExAllocatePoolWithTag(NonPagedPool,
	//											 sizeof(TEXTBUFF_INFO),
	//											 'nacS');
	//	if (NULL == notification)
	//	{
	//		status = STATUS_INSUFFICIENT_RESOURCES;
	//		leave;
	//	}

		// Read the beginning of the file and pass the contents to user mode
		offset.QuadPart = bytesRead = 0;
		//__debugbreak();
		status = FltReadFile(Instance,
				 FileObject,
				 &offset,
				 length,
				 buffer,
				 FLTFL_IO_OPERATION_NON_CACHED |
				 FLTFL_IO_OPERATION_DO_NOT_UPDATE_BYTE_OFFSET,
				 &bytesRead,
				 NULL,
				 NULL);
							  
		// LOG(" > > > FILE CONTENTS: %s\n", buffer);
		
		if(NT_SUCCESS(status) && (0 != bytesRead))
		{
			notification.BytesToScan = (ULONG)bytesRead;
			notification.Command = cmdSendTextBuff;
			notification.Allowed = *SafeToOpen;
			// Copy only as much as the buffer cand hold
			RtlCopyMemory(&notification.Contents,
				buffer,
				min(notification.BytesToScan, SCANNER_READ_BUFFER_SIZE));

			replyLength = sizeof(TEXTBUFF_INFO);

			status = FltSendMessage(gDrv.FilterHandle,
									(PFLT_PORT*)&gDrv.DllConnClientPort,
									&notification,
									sizeof(TEXTBUFF_INFO),
									&notification,
	     							&replyLength,
									NULL);

			if (STATUS_SUCCESS == status)
			{
				if (notification.Allowed == TRUE)
					LOG("+++++++++++++++++++++++++FILTER RECEIVED TRUE\n");
				else if (notification.Allowed == FALSE)
					LOG("-------------------------FILTER RECEIVED TRUE\n");
				*SafeToOpen = notification.Allowed;
			}
			else
			{
				LOG_ERROR("FltSendMessage failed with status 0x%x \n", status);
			}
		}
	}
	finally{

	}
	return status;
}



NTSTATUS
AvScan(
	_Inout_  PFLT_CALLBACK_DATA    Data,
	_In_     PCFLT_RELATED_OBJECTS FltObjects,
	_In_     AV_SCAN_MODE          ScanMode,
	_In_     UCHAR                 IOMajorFunctionAtScan,
	_In_     BOOLEAN               IsInTxWriter,
	_Inout_  PAV_STREAM_CONTEXT    StreamContext
)
{

}
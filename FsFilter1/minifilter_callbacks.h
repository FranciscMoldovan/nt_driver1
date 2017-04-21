#ifndef _MINIFILTER_CALLBACKS_H_
#define _MINIFILTER_CALLBACKS_H_

#include "driver1.h"

//////////////////////////////////////////////
typedef enum _AV_SCAN_MODE {

	//
	//  AvKernelMode indicates the scanning occurs in the kernel, while
	//  AvUserMode indicates the scanning happens in the user space.
	//

	AvKernelMode,
	AvUserMode

} AV_SCAN_MODE;
typedef union _AV_FILE_REFERENCE {

	struct {
		ULONGLONG   Value;
		ULONGLONG   UpperZeroes;
	} FileId64;

	FILE_ID_128     FileId128;

} AV_FILE_REFERENCE, *PAV_FILE_REFERENCE;
typedef struct _AV_TRANSACTION_CONTEXT {

	//
	//  Transaction object pointer
	//

	PKTRANSACTION Transaction;

	//
	//  List head for stream context list.
	//

	LIST_ENTRY        ScListHead;

	//
	//  Lock used to protect this context.
	//

	PERESOURCE Resource;

	//
	//  A flag that tracks:
	//    AV_TXCTX_ENLISTED: if it has been enlisted in transaction
	//    AV_TXCTX_LISTDRAINED: list is drained.
	//

	ULONG Flags;

} AV_TRANSACTION_CONTEXT, *PAV_TRANSACTION_CONTEXT;
typedef struct _AV_STREAM_CONTEXT {

	//
	//  Stream flags
	//

	ULONG Flags;

	//
	//  File ID, obtained from querying the file system for
	//  FileInternalInformation or FileIdInformation.
	//

	AV_FILE_REFERENCE FileId;

	//
	//  A pointer to the transaction context, so we can jump to list in the transaction.
	//

	PAV_TRANSACTION_CONTEXT  TxContext;

	//
	//  This list entry is exactly the embedded entry to 
	//  form a doubly linked list inside transaction context.
	//

	LIST_ENTRY  ListInTransaction;

	//
	//  We need to synchronize the creation of the section object. 
	//  If this syncrhonization is not made, FltCreateSectionForDataScan 
	//  would return STATUS_FLT_CONTEXT_ALREADY_DEFINED when two threads 
	//  are about to create the section for the same file.
	//

	PKEVENT    ScanSynchronizationEvent;

	//
	//  Please see AV_FILE_INFECTED_STATE for the definition of file state
	//  Note that we have TxState to maintain the isolation of 
	//  the transacted writer's view.
	//

	volatile LONG  State;


	volatile LONG  TxState;

	//
	// Revision numbers for files on CSVFS
	//
	LONGLONG   VolumeRevision;
	LONGLONG   CacheRevision;
	LONGLONG   FileRevision;

} AV_STREAM_CONTEXT, *PAV_STREAM_CONTEXT;
//////////////////////////////////////////////



NTSTATUS
AvScan(
	_Inout_  PFLT_CALLBACK_DATA    Data,
	_In_     PCFLT_RELATED_OBJECTS FltObjects,
	_In_     AV_SCAN_MODE          ScanMode,
	_In_     UCHAR                 IOMajorFunctionAtScan,
	_In_     BOOLEAN               IsInTxWriter,
	_Inout_  PAV_STREAM_CONTEXT    StreamContext
);

BOOLEAN
DoRequestOperationStatus(
    _In_ PFLT_CALLBACK_DATA Data
);

NTSTATUS
InstanceSetup(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
);

VOID
InstanceTeardownStart(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
);

VOID
InstanceTeardownComplete(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
);

NTSTATUS
InstanceQueryTeardown(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
);

FLT_PREOP_CALLBACK_STATUS
PreOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);

VOID
OperationStatusCallback(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
    _In_ NTSTATUS OperationStatus,
    _In_ PVOID RequesterContext
);

FLT_POSTOP_CALLBACK_STATUS
PostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
);

FLT_PREOP_CALLBACK_STATUS
PreOperationNoPostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);

#endif //_MINIFILTER_CALLBACKS_H_

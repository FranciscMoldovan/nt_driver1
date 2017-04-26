#ifndef _SERVER_COMMUNICATION_H_
#define _SERVER_COMMUNICATION_H_

#include <ntstatus.h>

NTSTATUS
InitializeCommunication();

void
UninitializeCommunication();

typedef struct _SCAN_SECTION_CONTEXT {
	HANDLE SectionHandle;
	PVOID  SectionObject;
	BOOLEAN Abordted;
	LONGLONG FileSize;
	BOOLEAN SectionCreated;
	BOOLEAN CancelableOnConflictingIo;
	PVOID ScanContext;
}SCAN_SECTION_CONTEXT, *PSCAN_SECTION_CONTEXT;

#endif //_SERVER_COMMUNICATION_H_
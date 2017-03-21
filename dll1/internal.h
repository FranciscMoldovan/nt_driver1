#ifndef _INTERNAL_H_
#define _INTERNAL_H_

//
// internal definitions and structues
//
#define MAX_DLLCONN_THREAD_COUNT            32

#define MESSAGE_BUFFER_SIZE                 65536 + 1024    // maximum size of a command buffer (65K shall be enough for any wide file name + command)

typedef struct _DLLCONN_THREAD_CONTEXT
{
    HANDLE Thread;
    DWORD Index;
    OVERLAPPED Overlapped;
    BYTE MessageBuffer[MESSAGE_BUFFER_SIZE];
} DLLCONN_THREAD_CONTEXT, *PDLLCONN_THREAD_CONTEXT;


////
//// prototypes
////
//NTSTATUS
//InternalStartLibrary(
//    __in DWORD NrOfDllConnThreads,
//    __in void *CallbackContext );
//
//NTSTATUS
//InternalStopLibrary(
//    void );
//
//NTSTATUS
//InternalSetOption(
//    __in DWORD Opt,
//    __in DWORD Flags,
//    __in QWORD Value );
//
//NTSTATUS
//InternalGetOption(
//    __in DWORD Opt,
//    __out_opt DWORD *Flags,
//    __out QWORD *Value );

#endif // _INTERNAL_H_

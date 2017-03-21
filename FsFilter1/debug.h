#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "driver1.h"

#define TAG_GLB                 'BLG:'      // global data
#define TAG_STR                 'RTS:'      // string buffer (for UNICODE_STRING)
#define TAG_BUF                 'FUB:'      // temporary buffer (for various query information calls)
#define TAG_NTF                 'FTN:'      // notify command

//PUCHAR 
//DrvNtStatusToString(
//    __in NTSTATUS Status );
//
//void
//DrvDumpMemory(
//    __in PVOID Address,
//    __in DWORD Length );
//
//DWORD
//DrvGetShortHexTimestamp(
//    void );

#endif // _DEBUG_H_
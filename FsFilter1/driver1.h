#ifndef _DRIVER1_H_
#define _DRIVER1_H_

#include <fltKernel.h>
#include <wdm.h>

#include "defs.h"
#include "debug.h"

#define LASTFILECHARS(MaxChars) ( (sizeof(__FILE__)-1 <= (MaxChars)) ? (__FILE__) : ((__FILE__) + sizeof(__FILE__)-1-(MaxChars)) )

#define LOG(format, ...) DbgPrint("%s : %d [%s] " ##":"##format, LASTFILECHARS(18), __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_INFO(format, ...) DbgPrint("%s : %d [%s] [INFO]" ##":"##format, LASTFILECHARS(18), __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_WARNING(format, ...) DbgPrint("%s : %d [%s] [WARNING]"##":"##format, LASTFILECHARS(18), __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERROR(format, ...) DbgPrint("%s : %d [%s] [ERROR]" ##":"##format,LASTFILECHARS(18), __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_CRITICAL(format, ...) DbgPrint("%s : %d [%s] [CRITICAL]"##":"##format,LASTFILECHARS(18), __LINE__, __FUNCTION__, __VA_ARGS__)

#define DRIVER_NAME             "driver1.sys"
#define FILTER_NAME             L"driver1"
#define DRIVER1_ALTITUDE        L"320801"       // this is for testing only; for real drivers, MS allocates an altitude

typedef struct _DRV_GLOBAL_DATA
{
    PDRIVER_OBJECT          DriverObject;
    PFLT_FILTER             FilterHandle;
    BOOLEAN                 ShuttingDown;
    PFLT_PORT               DllConnServerPort;
    struct _FLT_PORT *volatile     DllConnClientPort;
    KEVENT                  DllConnEvent;

} DRV_GLOBAL_DATA, *PDRV_GLOBAL_DATA;

extern PDRV_GLOBAL_DATA                 G;

void
DrvPreinitCommPort(
    void );

NTSTATUS
DrvInitializeCommPort(
    void );

NTSTATUS
DrvUninitializeCommPort(
    void );

void
DrvPreinitProcNotify(
    void );

NTSTATUS
DrvInitializeProcNotify(
    void );

NTSTATUS
DrvUninitializeProcNotify(
    void );

#endif // _DRIVER1_H_
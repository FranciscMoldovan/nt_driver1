//#ifndef _DEFS_H_
//#define _DEFS_H_
//
//// preprocessor macro for int ==> string coversion
//#define PREPROCSTR2(x)          #x
//#define PREPROCSTR(x)           PREPROCSTR2(x)
//
//#define LASTFILECHARS(MaxChars) ( (sizeof(__FILE__)-1 <= (MaxChars)) ? (__FILE__) : ((__FILE__) + sizeof(__FILE__)-1-(MaxChars)) )
//
//#define LOG_INFO(format, ...) DbgPrint("%s : %d [%s] [INFO]" ##":"##format, LASTFILECHARS(14), __LINE__, __FUNCTION__, __VA_ARGS__)
//#define LOG_WARNING(format, ...) DbgPrint("%s : %d [%s] [WARNING]"##":"##format, LASTFILECHARS(14), __LINE__, __FUNCTION__, __VA_ARGS__)
//#define LOG_ERROR(format, ...) DbgPrint("%s : %d [%s] [ERROR]" ##":"##format,LASTFILECHARS(14), __LINE__, __FUNCTION__, __VA_ARGS__)
//#define LOG_CRITICAL(format, ...) DbgPrint("%s : %d [%s] [CRITICAL]"##":"##format,LASTFILECHARS(14), __LINE__, __FUNCTION__, __VA_ARGS__)
//
////
//// enabled warnings
////
//#pragma warning(error:4100)     // unreferenced formal parameter
//#pragma warning(error:4101)     // unreferenced local variable
//#pragma warning(error:4061)     // missing enumeration in switch statement
//#pragma warning(error:4505)     // identify dead functions
//
//#include <fltKernel.h>
//#include <ntstrsafe.h>
//#include <dontuse.h>
//#include <suppress.h>
//
//#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")
//
//// define DEBUG (needed for DDK compiler)
//#ifdef DBG
//#ifndef DEBUG
//#define DEBUG                    1
//#endif
//#endif
//
//// standard types
//typedef unsigned __int8          BYTE, *PBYTE;
//typedef unsigned __int16         WORD, *PWORD;
//typedef unsigned __int32         *PDWORD;
//typedef unsigned __int64         QWORD, *PQWORD;
//
//
////
//// flag (single bit) and field (multiple bits) related macros
//// NOTE: SetFlag and ClearFlag might be already defined
////
//#define QueryFlag(Flag, Mask)               (((Flag) & (Mask)) != 0)
//#ifndef SetFlag
//    #define SetFlag(Flag, Mask)             { (Flag) |= (Mask); }
//#endif
//#ifndef ClearFlag
//    #define ClearFlag(Flag, Mask)           { (Flag) &= (~Mask); }
//#endif
//#define QueryFlagStr(Flag, Mask)            (((Flag) & (Mask))?"TRUE":"")
//
//// field related macros
//#define QueryField(Flag, Mask, Value)       (((Flag) & (Mask)) == (Value))
//#define GetField(Flag, Mask)                ((Flag) & (Mask))
//#define SetField(Flag, Mask, Value)         { (Flag) &= (~Mask); (Flag) |= (Value); }
//#define ClearField(Flag, Mask)              (Flag) &= (~Mask)
//
//// interlocked flag (single bit) macros
//#define QueryFlagInterlocked(Flag, Mask)    (((Flag) & (Mask)) != 0)        // we have no reason to use any
//                                                                            // interlocked stuff for a query
//#define SetFlagInterlocked(Flag, Mask)      (VOID)InterlockedOr((volatile LONG*)&Flag, (Mask))      // we use & to be consistent with
//#define ClearFlagInterlocked(Flag, Mask)    (VOID)InterlockedAnd((volatile LONG*)&Flag, ~(Mask))    // non-interlocked SetFlag() stuff
//
//
////
//// spin lock / mutex structure and macros
////
//// NOTE: we generally use spin locks, except when the lock is held for a long time
//// or, when the lock is acquired we can't afford a higher IRQL than PASSIVE_LEVEL
////
//typedef enum _DRV_LOCKTYPE {
//    lckSPINLOCK,
//    lckMUTEX,
//    lckERESOURCE,
//    lckPUSHLOCK
//} DRV_LOCKTYPE;
//
//#pragma warning(disable:4201)  // nonstandard extension used : nameless struct/union
//
//typedef struct _DRV_LOCK
//{
//    // type of the lock (spinlock, mutex or eresource)
//    DRV_LOCKTYPE                Type;
//    BOOLEAN                     Initialized;
//    PETHREAD                    Owner;
//    union
//    {
//        struct {
//            KSPIN_LOCK          SpinLock;
//            KIRQL               Irql;
//        };
//        struct {
//            KMUTEX              Mutex;
//        };
//        struct {
//            ERESOURCE           Resource;
//        };
//        struct {
//            EX_PUSH_LOCK        PushLock;
//        };
//    };
//} DRV_LOCK, *PDRV_LOCK;
//
//#pragma warning(default:4201)   // nonstandard extension used : nameless struct/union
//
//
////
//// PreInitLock - preinitialize lock
////
//#define PreInitLock(Lock)       {                                                                                       \
//                                    Lock.Initialized = FALSE;                                                           \
//                                }
//
////
//// InitLock - initalize lock structure
////
//#define InitLock(Lock, LckType) {                                                                                       \
//                                    Lock.Type = LckType;                                                                \
//                                    Lock.Owner = NULL;                                                                  \
//                                    if (Lock.Type == lckSPINLOCK)                                                       \
//                                        KeInitializeSpinLock(&(Lock.SpinLock));                                         \
//                                    else if (Lock.Type == lckMUTEX)                                                     \
//                                        KeInitializeMutex(&(Lock.Mutex), 0);                                            \
//                                    else if (Lock.Type == lckERESOURCE)                                                 \
//                                        ExInitializeResourceLite(&(Lock.Resource));                                     \
//                                    else if (Lock.Type == lckPUSHLOCK)                                                  \
//                                        FltInitializePushLock(&(Lock.PushLock));                                        \
//                                    Lock.Initialized = TRUE;                                                            \
//                                }
//
////
//// AcquireLock - acquire lock in exclusive mode
////
/////
///// CRITICAL NOTE: there is a significant issue with those macros: they do assume an always-successful acquire (for ex MUTEX)
/////
//#define AcquireLock(Lock)       {                                                                                       \
//                                    if (Lock.Type == lckSPINLOCK)                                                       \
//                                    {                                                                                   \
//                                        KeAcquireSpinLock(&(Lock.SpinLock), &(Lock.Irql));                              \
//                                        Lock.Owner = PsGetCurrentThread();                                              \
//                                    }                                                                                   \
//                                    else if (Lock.Type == lckMUTEX)                                                     \
//                                    {                                                                                   \
//                                        KeWaitForSingleObject(&(Lock.Mutex), Executive, KernelMode, FALSE, NULL);       \
//                                        Lock.Owner = PsGetCurrentThread();                                              \
//                                    }                                                                                   \
//                                    else if (Lock.Type == lckERESOURCE)                                                 \
//                                    {                                                                                   \
//                                        KeEnterCriticalRegion();                                                        \
//                                        ExAcquireResourceExclusiveLite(&(Lock.Resource), TRUE);                         \
//                                        Lock.Owner = PsGetCurrentThread();                                              \
//                                    }                                                                                   \
//                                    else if (Lock.Type == lckPUSHLOCK)                                                  \
//                                    {                                                                                   \
//                                        FltAcquirePushLockExclusive(&(Lock.PushLock));                                  \
//                                        Lock.Owner = PsGetCurrentThread();                                              \
//                                    }                                                                                   \
//                                }
//
//#define AcquireLockExclusive(Lock)  AcquireLock(Lock)
//
////
//// TryToAcquireLock - try to acquire lock in exclusive mode (ONLY for MUTEX-es !!!)
////
//extern LARGE_INTEGER gZeroTimeout;
//
//#define TryToAcquireLock(Lock)      (Lock.Type == lckSPINLOCK)?(FALSE):                                                 \
//                                    (Lock.Type == lckERESOURCE)?(FALSE):                                                \
//                                    (Lock.Type == lckPUSHLOCK)?(FALSE):                                                 \
//                                    (Lock.Type != lckMUTEX)?(FALSE):                                                    \
//                                        ((STATUS_TIMEOUT == KeWaitForSingleObject(&(Lock.Mutex), Executive, KernelMode, FALSE, &gZeroTimeout))?(FALSE):(TRUE))
//
////
//// AcquireLockShared - acuire lock in shared mode
////
//// IMPORTANT: shared is only possible for ERESOURCE and PUSHLOCK
////
//#define AcquireLockShared(Lock) {                                                                                       \
//                                    if (Lock.Type == lckSPINLOCK)                                                       \
//                                    {                                                                                   \
//                                        KeAcquireSpinLock(&(Lock.SpinLock), &(Lock.Irql));                              \
//                                        Lock.Owner = PsGetCurrentThread();                                              \
//                                    }                                                                                   \
//                                    else if (Lock.Type == lckMUTEX)                                                     \
//                                    {                                                                                   \
//                                        KeWaitForSingleObject(&(Lock.Mutex), Executive, KernelMode, FALSE, NULL);       \
//                                        Lock.Owner = PsGetCurrentThread();                                              \
//                                    }                                                                                   \
//                                    else if (Lock.Type == lckERESOURCE)                                                 \
//                                    {                                                                                   \
//                                        KeEnterCriticalRegion();                                                        \
//                                        ExAcquireResourceSharedLite(&(Lock.Resource), TRUE);                            \
//                                        /* for shared acquire we do NOT set Owner */                                    \
//                                    }                                                                                   \
//                                    else if (Lock.Type == lckPUSHLOCK)                                                  \
//                                    {                                                                                   \
//                                        FltAcquirePushLockShared(&(Lock.PushLock));                                     \
//                                        /* for shared acquire we do NOT set Owner */                                    \
//                                    }                                                                                   \
//                                }
//
////
//// ReleaseLock - release lock
////
//// IMPORTANT: for Lock.Owner flag to be cleared correctly, any exclusively acquired
//// lock MUST be released in the same thread context, in which was acquired
////
//#define ReleaseLock(Lock)       {                                                                                       \
//                                    if (Lock.Type == lckSPINLOCK)                                                       \
//                                    {                                                                                   \
//                                        Lock.Owner = NULL;                                                              \
//                                        KeReleaseSpinLock(&(Lock.SpinLock), Lock.Irql);                                 \
//                                    }                                                                                   \
//                                    else if (Lock.Type == lckMUTEX)                                                     \
//                                    {                                                                                   \
//                                        Lock.Owner = NULL;                                                              \
//                                        KeReleaseMutex(&(Lock.Mutex), FALSE);                                           \
//                                    }                                                                                   \
//                                    else if (Lock.Type == lckERESOURCE)                                                 \
//                                    {                                                                                   \
//                                        if (PsGetCurrentThread() == Lock.Owner)                                         \
//                                            Lock.Owner = NULL;                                                          \
//                                        ExReleaseResourceLite(&(Lock.Resource));                                        \
//                                        KeLeaveCriticalRegion();                                                        \
//                                    }                                                                                   \
//                                    else if (Lock.Type == lckPUSHLOCK)                                                  \
//                                    {                                                                                   \
//                                        if (PsGetCurrentThread() == Lock.Owner)                                         \
//                                            Lock.Owner = NULL;                                                          \
//                                        FltReleasePushLock(&(Lock.PushLock));                                           \
//                                    }                                                                                   \
//                                }
//
////
//// IMPORTANT: we disable PREfast warning 28107: The CriticalRegion '#CriticalRegion' must
//// be held when calling 'ExReleaseResourceLite', because PREfast can't parse correctly the
//// DRV_LOCK structure and threats all DRV_LOCKs as beeing ERESOURCEs
////
//#pragma prefast(disable: 28107, "PREfast: PREfast can't parse correctly DRV_LOCK structures")
//
////
//// DeleteLock
////
//#define DeleteLock(Lock)        {                                                                                       \
//                                    if (Lock.Type == lckSPINLOCK)                                                       \
//                                        /* nothing needed for spinlocks */;                                             \
//                                    else if (Lock.Type == lckMUTEX)                                                     \
//                                        /* nothing needed for eresources */;                                            \
//                                    else if ((Lock.Type == lckERESOURCE) && (Lock.Initialized))                         \
//                                        ExDeleteResourceLite(&(Lock.Resource));                                         \
//                                    else if ((Lock.Type == lckPUSHLOCK) && (Lock.Initialized))                          \
//                                        FltDeletePushLock(&(Lock.PushLock));                                            \
//                                    Lock.Initialized = FALSE;                                                           \
//                                }
//
//
////
//// delay values for KeDelayExecutionThread()
////
//// NOTE: values are negative to represent relative time
////
//#define DELAY_ONE_MICROSECOND               (-10)
//#define DELAY_ONE_MILLISECOND               (DELAY_ONE_MICROSECOND*1000)
//#define DELAY_ONE_SECOND                    (DELAY_ONE_MILLISECOND*1000)
//
//#endif // _DEFS_H_
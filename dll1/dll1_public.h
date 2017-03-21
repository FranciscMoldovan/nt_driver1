#ifndef _DLL1_PUBLIC_H_
#define _DLL1_PUBLIC_H_


#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport) 
#else
#define DLL_API  
#endif

#define SUCCESS(Status)                  (((__int32)(Status)) >= 0)

#ifdef __cplusplus
extern "C" {
#endif // C++


    typedef
        __int32
        (__cdecl FUNC_InitializeDriverLibrary)(
            void
            );
    typedef   FUNC_InitializeDriverLibrary*   PFUNC_InitializeDriverLibrary;

    DLL_API   FUNC_InitializeDriverLibrary    InitializeDriverLibrary;

    typedef
        __int32
        (__cdecl FUNC_UninitializeDriverLibrary)(
            void
            );
    typedef   FUNC_UninitializeDriverLibrary*   PFUNC_UninitializeDriverLibrary;

    DLL_API   FUNC_UninitializeDriverLibrary    UninitializeDriverLibrary;


    typedef
        __int32
        (__cdecl FUNC_DexTestCommand)(
            void
            );
    typedef   FUNC_DexTestCommand*   PFUNC_DexTestCommand;

    DLL_API   FUNC_DexTestCommand    DexTestCommand;



////
//// driver and service related
////
//DLL_API __int32
//DexQueryDriver(
//    __out BOOLEAN *IsRegistered,
//    __out BOOLEAN *IsRunning );
//
//DLL_API __int32
//DexStartDriver(
//    __in BOOLEAN RestartIfRunning,
//    __out BOOLEAN *WasRunning );
//
//DLL_API __int32
//DexStopDriver(
//    void );

DLL_API
void
LogPrint2(
    char*   SourceFile,
    __int32 SourceLine,
    char*   FunctionName,
    const char* Format,
    ...
);

#define LogPrint(format, ...)           LogPrint2(__FILE__, __LINE__, __FUNCTION__, format, __VA_ARGS__)
#define LogPrintNaked(format, ...)      LogPrint2(NULL, 0, NULL, format, __VA_ARGS__)                                                                 \

#ifdef __cplusplus
}
#endif // C++


#endif // _DLL1_PUBLIC_H_
#include "dll1.h"
#include "internal.h"

//
// DllMain
//
BOOL APIENTRY
DllMain(
    __in HMODULE hModule,
    __in DWORD ul_reason_for_call,
    __in LPVOID lpReserved
	)
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


//
// DexQueryDriver
//
DLL_API
NTSTATUS
DexQueryDriver(
    __out BOOLEAN *IsRegistered,
    __out BOOLEAN *IsRunning 
    )
{
    NTSTATUS status;

    status = STATUS_UNSUCCESSFUL;

    if (NULL == IsRegistered)
    {
        return STATUS_INVALID_PARAMETER_1;
    }

    if (NULL == IsRunning)
    {
        return STATUS_INVALID_PARAMETER_2;
    }

    // check IsRegistered based on a registry key under HKLM\\SYSTEM\CurrentControlSet\services\DRIVER1
    // NOTE: we MUST avoid using SCM for query, because it has many issues
    {
        HRESULT res;
        HKEY key;

        res = RegOpenKeyEx(
                HKEY_LOCAL_MACHINE, 
                L"SYSTEM\\CurrentControlSet\\services\\DRIVER1",
                0,
                KEY_QUERY_VALUE, 
                &key );
        if (ERROR_SUCCESS == res)
        {
            *IsRegistered = TRUE;
            RegCloseKey(key);
        }
        else
        {
            LogPrint("WARNING: RegOpenKeyEx failed, result=0x%08x\n", res);
            *IsRegistered = FALSE;
        }
    }

    // now, check using filter manager APIs if DRIVER1 is running
    {
        HRESULT res;
        HFILTER filter;

        res = FilterCreate(L"DRIVER1", &filter);
        if (S_OK == res)
        {
            *IsRunning = TRUE;
            FilterClose(filter);
        }
        else
        {
            if (0x801f0013 != res)
            {
                LogPrint("WARNING: FilterCreate failed, result=0x%08x\n", res);
            }
            *IsRunning = FALSE;
        }
    }

    return STATUS_SUCCESS;
}

DLL_API NTSTATUS
DexStopDriver(
    void)
{
    HRESULT res;
    res = FilterUnload(L"DRIVER1");
    if (S_OK != res)
    {
        LogPrint("ERROR: FilterUnload failed, res=0x%08x\n", res);
        return STATUS_UNSUCCESSFUL;
    }
    return STATUS_SUCCESS;
}

DLL_API NTSTATUS
DexStartDriver(
    __in BOOLEAN RestartIfRunning,
    __out BOOLEAN *WasRunning 
    )
{
    NTSTATUS status;
    BOOLEAN isRegistered;
    BOOLEAN isRunning;

    if (NULL == WasRunning)
    {
        return STATUS_INVALID_PARAMETER_2;
    }

    status = DexQueryDriver(&isRegistered, &isRunning);
    if (!SUCCESS(status))
    {
        return status;
    }

    *WasRunning = isRunning;

    // stop driver if needed
    if (isRunning && (TRUE == RestartIfRunning))
    {
        status = DexStopDriver();
        if (!SUCCESS(status))
        {
            return status;
        }

        isRunning = FALSE;
    }

    // start driver if needed
    if (FALSE == isRunning)
    {
        HRESULT res;

        res = FilterLoad(L"DRIVER1");
        if (S_OK != res)
        {
            LogPrint("ERROR: FilterLoad failed, res=0x%08x\n", res);
            return STATUS_UNSUCCESSFUL;
        }
    }

    return STATUS_SUCCESS;
}    



//DLL_API NTSTATUS
//InitializeDriverLibrary(
//    __in BOOLEAN StartDriverIfNeeded,
//    __in DWORD NrOfDllConnThreads,
//    __in void *Reserved,
//    __in void *CallbackContext 
//    )
//{
//    NTSTATUS status;
//
//    UNREFERENCED_PARAMETER(Reserved);
//
//    status = STATUS_UNSUCCESSFUL;
//
//    if (StartDriverIfNeeded)
//    {
//        BOOLEAN wasRunning;
//
//        status = DexStartDriver(FALSE, &wasRunning);
//        if (!SUCCESS(status))
//        {
//            LogPrint("ERROR: DexStartDriver failed, status=0x%08x\n", status);
//            return status;
//        }
//    }
//
//    status = InternalStartLibrary(NrOfDllConnThreads, CallbackContext);
//
//    return status;
//}




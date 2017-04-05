#include "dll1_public.h"
#include <windows.h>
#include <iostream>
#include <string>
//#include <sstream>

using namespace std;

int main()
{
    HMODULE dllModule = LoadLibraryW(L"dll1.dll");
    PFUNC_InitializeDriverLibrary InitializeDriverLibrary;
    PFUNC_UninitializeDriverLibrary UninitializeDriverLibrary;
    PFUNC_DexTestCommand TestCommand;
	PFUNC_DexToggleProcessMonitoring ToggleProcessMonitoring;

    if (NULL == dllModule)
    {
        cout << "failed loading dll1.dll last error " << GetLastError();
        return -1;
    }

    InitializeDriverLibrary = (PFUNC_InitializeDriverLibrary)GetProcAddress(dllModule, "InitializeDriverLibrary");
    if (NULL == InitializeDriverLibrary)
    {
        cout << "failed loading InitializeDriverLibrary \n";
        return -1;
    }

    UninitializeDriverLibrary = (PFUNC_UninitializeDriverLibrary)GetProcAddress(dllModule, "UninitializeDriverLibrary");
    if (NULL == UninitializeDriverLibrary)
    {
        cout << "failed loading UninitializeDriverLibrary \n";
        return -1;
    }

    __int32 status = InitializeDriverLibrary();
    cout << "InitializeDriverLibrary returned " << status << endl;

    TestCommand = (PFUNC_DexTestCommand)GetProcAddress(dllModule, "DexTestCommand");
    if (NULL == TestCommand)
    {
        cout << "failed loading DexTestCommand \n";
        return -1;
    }

	ToggleProcessMonitoring = (PFUNC_DexToggleProcessMonitoring)GetProcAddress(dllModule, "DexToggleProcessMonitoring");
	if (NULL == ToggleProcessMonitoring)
	{
		cout << "failed loading DexToggleProcessMonitoring \n";
		return -1;
	}

    while (true)
    {
        string command;
        cin >> command;
        cout << "read command: " << command << endl;

        if (command == "exit")
        {
            break;
        }

        if (command == "test")
        {
            TestCommand();
        }

		if (command == "procmon_on")
		{
			ToggleProcessMonitoring(1);
		}

		if (command == "procmon_off")
		{
			ToggleProcessMonitoring(0);
		}
    }

    UninitializeDriverLibrary();
}

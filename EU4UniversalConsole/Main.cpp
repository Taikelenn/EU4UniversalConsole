#include <Windows.h>

#include "EU4Offsets.h"
#include "HookManager.h"

HINSTANCE g_hInstDLL;

DWORD CALLBACK InitThread(LPVOID lpParameter)
{
	UNREFERENCED_PARAMETER(lpParameter);
	
	HWND windowHandle = nullptr;
	while ((windowHandle = EU4Offsets::GetWindowHandle()) == nullptr)
	{
		Sleep(200);
	}

	HookManager::InstallHooks(windowHandle);

	// a beep to signal that injection has finished
	Beep(777, 100);

	if (WaitForSingleObject(HookManager::hUnloadEvent, INFINITE) == WAIT_OBJECT_0)
	{
		HookManager::UninstallHooks(windowHandle);
	}

	return ERROR_SUCCESS;
}

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		g_hInstDLL = hInstDLL; // store the DLL address for potential unloading

		// perform initialization in separate thread since it's easy to run into a deadlock in DllMain
		HANDLE hThread = CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
		if (hThread)
		{
			CloseHandle(hThread);
		}
	}

	return TRUE;
}

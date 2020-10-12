#pragma once

#include <Windows.h>

namespace HookManager {
	// Hooks the necessary EU4 functions.
	void InstallHooks(HWND hwnd);

	// Removes hooks placed by InstallHooks().
	void UninstallHooks(HWND hwnd);

	// A handle to an unnamed event. When signaled, the main DLL thread (InitThread) unloads the console.
	extern HANDLE hUnloadEvent;
}

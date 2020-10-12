#pragma once

#include <Windows.h>

namespace HookManager {
	// Hooks the necessary EU4 functions.
	void InstallHooks(HWND hwnd);

	// Removes hooks placed by InstallHooks().
	void UninstallHooks(HWND hwnd);
}

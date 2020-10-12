#include "HookManager.h"

static WNDPROC previousWndProc;
LRESULT CALLBACK hookedWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc(previousWndProc, hwnd, uMsg, wParam, lParam);
}

bool hasHookedWndProc;

bool hasHookedEndScene;

bool hasHookedGameIdle;

void HookWndProc(HWND hwnd)
{
	previousWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)hookedWindowProc);
	hasHookedWndProc = true;
}

void HookManager::InstallHooks(HWND hwnd)
{
	if (hwnd)
	{
		HookWndProc(hwnd);
	}
}

void HookManager::UninstallHooks(HWND hwnd)
{
	if (hasHookedWndProc && hwnd)
	{
		hasHookedWndProc = false;
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)previousWndProc);
	}
}

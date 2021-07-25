#include "HookManager.h"
#include "EU4Offsets.h"
#include "CommandExecutor.h"

#include "GfxMasterContextGFX.h"
#include "DrawingManager.h"

#include <cstdio>
#include <d3d9.h>

HANDLE HookManager::hUnloadEvent;

// we overwrite EU4 functions using these trampolines which will jump to our hooked routines
static unsigned char trampolineEndScene[] = { 0x48, 0xB8, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xE0, 0x90, 0x90, 0x90, 0x90, 0x90 };
static unsigned char trampolineGameIdle[] = { 0x48, 0xB8, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xE0 };

// the hooked procedure stubs - since x64 MSVC does not allow inline assembly, this is stored as an array of assembled x64 opcodes
static unsigned char hookProcedureEndScene[] = { 0x50, 0x53, 0x51, 0x52, 0x56, 0x57, 0x55, 0x41, 0x50, 0x41, 0x51, 0x41, 0x52, 0x41, 0x53, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00, 0x48, 0xB8, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFF, 0xD0, 0x41, 0x5F, 0x41, 0x5F, 0x41, 0x5F, 0x41, 0x5F, 0x41, 0x5F, 0x41, 0x5E, 0x41, 0x5D, 0x41, 0x5C, 0x41, 0x5B, 0x41, 0x5A, 0x41, 0x59, 0x41, 0x58, 0x5D, 0x5F, 0x5E, 0x5A, 0x59, 0x5B, 0x58, 0x48, 0x83, 0xEC, 0x28, 0x48, 0x8B, 0x49, 0x08, 0x48, 0x8B, 0x01, 0xFF, 0x90, 0x50, 0x01, 0x00, 0x00, 0x68, 0xAA, 0xAA, 0xAA, 0xAA, 0xC7, 0x44, 0x24, 0x04, 0xBB, 0xBB, 0xBB, 0xBB, 0xC3 };
static unsigned char hookProcedureGameIdle[] = { 0x50, 0x53, 0x51, 0x52, 0x56, 0x57, 0x55, 0x41, 0x50, 0x41, 0x51, 0x41, 0x52, 0x41, 0x53, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00, 0x48, 0xB8, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFF, 0xD0, 0x41, 0x5F, 0x41, 0x5F, 0x41, 0x5F, 0x41, 0x5F, 0x41, 0x5F, 0x41, 0x5E, 0x41, 0x5D, 0x41, 0x5C, 0x41, 0x5B, 0x41, 0x5A, 0x41, 0x59, 0x41, 0x58, 0x5D, 0x5F, 0x5E, 0x5A, 0x59, 0x5B, 0x58, 0x48, 0x89, 0xE0, 0x88, 0x50, 0x10, 0x48, 0x89, 0x48, 0x08, 0x55, 0x53, 0x68, 0xAA, 0xAA, 0xAA, 0xAA, 0xC7, 0x44, 0x24, 0x04, 0xBB, 0xBB, 0xBB, 0xBB, 0xC3 };

// =========== Hooked procedures ===========
static WNDPROC previousWndProc;
LRESULT CALLBACK hookedWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT retval;

	if (DrawingManager::WindowProc(hwnd, uMsg, wParam, lParam, &retval))
	{
		return retval;
	}

	return CallWindowProc(previousWndProc, hwnd, uMsg, wParam, lParam);
}

void hookedEndScene(GfxMasterContextGFX* ctx)
{
	IDirect3DDevice9* device = ctx->device;

	static bool isDrawingInitialized = false;
	if (!isDrawingInitialized)
	{
		isDrawingInitialized = true;
		DrawingManager::Initialize(device);
	}

	CommandExecutor::CheckForCommandTimeout();
	DrawingManager::RenderFrame(device);
}

void hookedGameIdle()
{
	if (CommandExecutor::commandScheduled)
	{
		CommandExecutor::ExecuteScheduledCommand();
	}
}

bool hasHookedWndProc;

bool hasHookedEndScene;
unsigned char endSceneOrigBytes[sizeof(trampolineEndScene)];

bool hasHookedGameIdle;
unsigned char gameIdleOrigBytes[sizeof(trampolineGameIdle)];

void HookWndProc(HWND hwnd)
{
	previousWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)hookedWindowProc);
	hasHookedWndProc = true;
}

void HookEndScene()
{
	void* func = EU4Offsets::TranslateOffset(EU4Offsets::EndSceneOffset);
	uint64_t retAddr = (uint64_t)func + sizeof(trampolineEndScene);

	// allocate executable memory for the hook stub
	void* hookStub = VirtualAlloc(NULL, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (hookStub == nullptr)
	{
		// if we fail, display an error message and unload
		char msgError[192];
		sprintf_s(msgError, __FUNCTION__ ": failed to allocate memory, error %d", GetLastError());

		MessageBoxA(nullptr, msgError, "Hooking failure", MB_OK | MB_ICONERROR);
		SetEvent(HookManager::hUnloadEvent);

		return;
	}

	// this level of indirection is necessary so we don't directly copy the hookedEndScene's code
	void* targetFunc = (void*)hookedEndScene;

	memcpy((unsigned char*)trampolineEndScene + 0x02, &hookStub, sizeof(void*)); // copy the hook stub's address to the trampoline
	memcpy((unsigned char*)hookProcedureEndScene + 0x21, &targetFunc, sizeof(void*)); // copy the actual hooked function's address to the hook stub

	*(uint32_t*)((unsigned char*)hookProcedureEndScene + 0x5C) = (uint32_t)(retAddr & 0xFFFFFFFF);
	*(uint32_t*)((unsigned char*)hookProcedureEndScene + 0x64) = (uint32_t)(retAddr >> 32);

	memcpy(hookStub, hookProcedureEndScene, sizeof(hookProcedureEndScene));

	DWORD oldProtect;
	VirtualProtect(func, sizeof(trampolineEndScene), PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy(endSceneOrigBytes, func, sizeof(endSceneOrigBytes));
	memcpy(func, trampolineEndScene, sizeof(trampolineEndScene));

	VirtualProtect(func, sizeof(trampolineEndScene), oldProtect, &oldProtect);
	hasHookedEndScene = true;
}

void HookGameIdle()
{
	void* func = EU4Offsets::TranslateOffset(EU4Offsets::GameIdleOffset);
	uint64_t retAddr = (uint64_t)func + sizeof(trampolineGameIdle);

	void* hookStub = VirtualAlloc(NULL, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (hookStub == nullptr)
	{
		char msgError[192];
		sprintf_s(msgError, __FUNCTION__ ": failed to allocate memory, error %d", GetLastError());

		MessageBoxA(nullptr, msgError, "Hooking failure", MB_OK | MB_ICONERROR);
		return;
	}

	void* targetFunc = (void*)hookedGameIdle;

	memcpy((unsigned char*)trampolineGameIdle + 0x02, &hookStub, sizeof(void*));
	memcpy((unsigned char*)hookProcedureGameIdle + 0x21, &targetFunc, sizeof(void*));

	*(uint32_t*)((unsigned char*)hookProcedureGameIdle + 0x57) = (uint32_t)(retAddr & 0xFFFFFFFF);
	*(uint32_t*)((unsigned char*)hookProcedureGameIdle + 0x5F) = (uint32_t)(retAddr >> 32);

	memcpy(hookStub, hookProcedureGameIdle, sizeof(hookProcedureGameIdle));

	DWORD oldProtect;
	VirtualProtect(func, sizeof(trampolineGameIdle), PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy(gameIdleOrigBytes, func, sizeof(gameIdleOrigBytes));
	memcpy(func, trampolineGameIdle, sizeof(trampolineGameIdle));

	VirtualProtect(func, sizeof(trampolineGameIdle), oldProtect, &oldProtect);
	hasHookedGameIdle = true;
}

void HookManager::InstallHooks(HWND hwnd)
{
	HookManager::hUnloadEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);

	if (hwnd)
	{
		HookWndProc(hwnd);
	}

	HookEndScene();
	HookGameIdle();
}

void HookManager::UninstallHooks(HWND hwnd)
{
	DWORD oldProtect;

	if (hasHookedWndProc && hwnd)
	{
		hasHookedWndProc = false;
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)previousWndProc);
	}

	if (hasHookedEndScene)
	{
		hasHookedEndScene = false;

		void* func = EU4Offsets::TranslateOffset(EU4Offsets::EndSceneOffset);
		VirtualProtect(func, sizeof(endSceneOrigBytes), PAGE_EXECUTE_READWRITE, &oldProtect);

		memcpy(func, endSceneOrigBytes, sizeof(endSceneOrigBytes));

		VirtualProtect(func, sizeof(endSceneOrigBytes), oldProtect, &oldProtect);
	}

	if (hasHookedGameIdle)
	{
		hasHookedGameIdle = false;

		void* func = EU4Offsets::TranslateOffset(EU4Offsets::GameIdleOffset);
		VirtualProtect(func, sizeof(gameIdleOrigBytes), PAGE_EXECUTE_READWRITE, &oldProtect);

		memcpy(func, gameIdleOrigBytes, sizeof(gameIdleOrigBytes));

		VirtualProtect(func, sizeof(gameIdleOrigBytes), oldProtect, &oldProtect);
	}
}

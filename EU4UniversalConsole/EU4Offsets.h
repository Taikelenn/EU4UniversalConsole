#pragma once

#include <cstddef>
#include <cstdint>

#include <Windows.h>

// Offsets updated for EU4 version 1.30.4 (5d62)
namespace EU4Offsets {
	// GfxEndSceneDX9 is hooked and used to draw the console.
	constexpr std::ptrdiff_t EndSceneOffset = 0x1345F20; // signature 48 83 EC 28 48 8B 49 08 48 8B 01 FF 90 50 01 00 00

	// CInGameIdler::Idle is hooked and used to execute console commands from the game's main thread. Executing commands from the EndScene function directly might cause threading issues
	constexpr std::ptrdiff_t GameIdleOffset = 0x70E100; // string reference "End game screen from country annexed" or, if that fails, signature 48 8B C4 88 50 10 55 53 56 57 41 54 41 55 41 56

	// Converts a relative offset into a memory address within eu4.exe's address space
	inline void* TranslateOffset(std::ptrdiff_t offset)
	{
		// GetModuleHandleA(nullptr) has no performance impact due to an explicit check for a nullptr argument, so it's not really beneficial to store the base address in a global variable.
		return (void*)(offset + (std::ptrdiff_t)GetModuleHandleA(nullptr));
	}

	// Gets the EU4 window handle (used in ImGui)
	inline HWND GetWindowHandle()
	{
		// This assumes that there's only one EU4 instance running.
		return FindWindowA("SDL_app", "Europa Universalis IV");
	}
}

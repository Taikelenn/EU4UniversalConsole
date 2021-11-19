#pragma once

#include <cstddef>
#include <cstdint>

#include <Windows.h>

class CCommandResult;
class CStringArray;
class SOldCommandData;

typedef CCommandResult* (*CommandFunction_t)(CCommandResult*, CStringArray*);

// Offsets updated for EU4 version 1.32.1.0 (78cb)
namespace EU4Offsets {
	// GfxEndSceneDX9 is hooked and used to draw the console.
	constexpr std::ptrdiff_t EndSceneOffset = 0x149A9D0; // signature 48 83 EC 28 48 8B 49 08 48 8B 01 FF 90 50 01 00 00

	// CInGameIdler::Idle is hooked and used to execute console commands from the game's main thread. Executing commands from the EndScene function directly might cause threading issues
	constexpr std::ptrdiff_t GameIdleOffset = 0x77D610; // string reference "End game screen from country annexed"

	// Speculation: the number of random number generations (the second value in the function)
	constexpr std::ptrdiff_t RandomCountOffset = 0x23F9AF4; // signature 8B 41 ?? 89 ?? ?? ?? ?? ?? 8B 41 ?? 89 ?? ?? ?? ?? ?? E9 ?? ?? ?? ??

	// Speculation: an index representing the value to get from the random number generator's internal state (the value set to 0 in the function)
	constexpr std::ptrdiff_t RandomIndexOffset = 0x23D2FE0; // signature 45 33 DB 44 89 ?? ?? ?? ?? ?? 85 C0 74 ??

	// Array of CConsoleCmd::SOldCommandData
	constexpr std::ptrdiff_t CommandListOffset = 0x20C76F0;
	constexpr int CommandCount = 371;

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

	inline SOldCommandData* GetCommandList()
	{
		return (SOldCommandData*)TranslateOffset(CommandListOffset);
	}

	// we exploit the fact that both RandomIndex and RandomCount are 4-byte, so we can pack that into one 8-byte variable
	inline uint64_t StoreRandomState()
	{
		uint64_t retval = *(uint32_t*)TranslateOffset(RandomIndexOffset);
		return (retval << 32) | *(uint32_t*)TranslateOffset(RandomCountOffset);
	}

	inline void RestoreRandomState(uint64_t randomState)
	{
		*(uint32_t*)TranslateOffset(RandomIndexOffset) = (randomState >> 32) & 0xFFFFFFFF;
		*(uint32_t*)TranslateOffset(RandomCountOffset) = randomState & 0xFFFFFFFF;
	}
}

#pragma once

#include <Windows.h>
#include <d3d9.h>

namespace DrawingManager {
	void Initialize(IDirect3DDevice9* device);
	void RenderFrame(IDirect3DDevice9* device);

	void Destroy();

	void RenderOverlay();

	bool WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lResultPtr);

	extern bool inputBlocked;
	extern bool listingCommands;
	extern bool allowDevCommands;
}

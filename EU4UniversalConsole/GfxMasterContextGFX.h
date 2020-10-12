#pragma once

#include <d3d9.h>

// a pointer to this class is passed as an argument to our hooked EndScene
class GfxMasterContextGFX {
public:
	void* unknown;
	IDirect3DDevice9* device;
};

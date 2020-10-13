#pragma once

#include <Windows.h>

namespace EU4Injector {
	bool IsEU4Process(DWORD processId);
	bool Inject(DWORD processId);
}

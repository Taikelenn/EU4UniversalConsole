#pragma once

#include <Windows.h>

constexpr wchar_t InjectedDLLName[] = L"EU4UniversalConsole.dll";

namespace EU4Injector {
	bool IsEU4Process(DWORD processId);
	bool Inject(DWORD processId);
}

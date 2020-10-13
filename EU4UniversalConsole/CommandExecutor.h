#pragma once

#include <string>
#include <Windows.h>

#include "CString.h"

class UIConsole;

class CCommandResult {
public:
	uint64_t isSuccessful;
	CString result;

	inline CCommandResult() { isSuccessful = 0; }
};

namespace CommandExecutor {
	void ExecuteCommand(const std::string& command, UIConsole* console);
	void ExecuteScheduledCommand();

	void CheckForCommandTimeout();
	void RemoveScheduledCommand();

	// if 0, no command is scheduled; otherwise a result of GetTickCount64() indicating when the command was scheduled
	// the least significant bit indicates whether the command is being currently executed
	extern ULONGLONG commandScheduled;
}

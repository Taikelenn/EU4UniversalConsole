#pragma once

#include <string>

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

	extern bool commandScheduled;
}

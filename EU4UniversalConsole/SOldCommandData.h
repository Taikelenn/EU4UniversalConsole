#pragma once

#include <cstdint>

#include "EU4Offsets.h"

class SOldCommandData {
public:
	uint64_t isAvailableInRelease;
	const char* commandName;
	uint64_t shortcutCount;
	const char* commandShortcut[3];
	const char* commandDescription;
	CommandFunction_t function;
	uint64_t argumentCount;
	const char* argumentDescription[10];
	uint64_t unknown;
};

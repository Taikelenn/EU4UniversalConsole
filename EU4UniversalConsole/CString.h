#pragma once

#include <string>

// this seems to be the same thing as std::string, but we write it from scratch to mirror the EU4's implementation
class CString
{
private:
	union {
		const char buf[16];
		const char* ptr;
	};

	size_t size;
	size_t allocSize;

public:
	CString(const std::string& s);

	inline const char* GetData() { if (size >= 0x10) return ptr; return (const char*)buf; }
	inline size_t GetSize() { return size; }
};

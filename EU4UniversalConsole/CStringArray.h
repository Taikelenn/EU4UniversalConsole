#pragma once

#include "CString.h"
#include <vector>

// this seems to be the same thing as std::vector, but we write it from scratch to mirror the EU4's implementation
class CStringArray
{
private:
	CString* first;
	CString* last;
	CString* end;

public:
	inline CStringArray() { first = last = end = nullptr; }
	CStringArray(const std::vector<CString>& vec);

	inline size_t GetElementCount() { return (size_t)(last - first) / sizeof(CString); }
};

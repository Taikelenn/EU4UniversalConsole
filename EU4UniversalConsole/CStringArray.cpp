#include "CStringArray.h"

#include <Windows.h>

CStringArray::CStringArray(const std::vector<CString>& vec)
{
	this->first = (CString*)HeapAlloc(GetProcessHeap(), 0, sizeof(CString) * vec.size());
	if (this->first == nullptr)
	{
		abort();
	}

	memcpy(this->first, vec.data(), vec.size() * sizeof(CString)); // CString is a POD-type

	this->last = this->first + vec.size();
	this->end = this->last;
}

#include "CString.h"

#include <Windows.h>

CString::CString(const std::string& s)
{
	// we mirror the EU4's way of allocating memory (which is internally calling HeapAlloc, though this might just as well be the msvcrt's default way of allocating memory)
	if (s.empty())
	{
		this->size = 0;
		this->allocSize = 15;
	}

	if (s.size() < 0x10)
	{
		strcpy((char*)this->buf, s.c_str());
		this->size = s.size();
		this->allocSize = 15;
	}
	else
	{
		this->ptr = (const char*)HeapAlloc(GetProcessHeap(), 0, s.size() + 1);
		if (this->ptr == nullptr)
		{
			abort();
		}

		strcpy((char*)this->ptr, s.c_str());

		this->size = s.size();
		this->allocSize = s.size() + 1;
	}
}

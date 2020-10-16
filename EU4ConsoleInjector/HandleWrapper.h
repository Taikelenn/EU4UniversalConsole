#pragma once

#include <Windows.h>

// a simple class for automatic release of Windows process handles
class HandleWrapper {
private:
	HANDLE handle;

public:
	inline HandleWrapper(HANDLE h) : handle(h) { }
	inline ~HandleWrapper() { if (handle) CloseHandle(handle); }

	HandleWrapper(const HandleWrapper& other) = delete;
	HandleWrapper& operator=(const HandleWrapper& other) = delete;

	inline explicit operator bool() const { return handle != nullptr; }

	inline HANDLE get() const { return handle; }
};

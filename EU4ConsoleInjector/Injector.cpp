#include "Injector.h"
#include "HandleWrapper.h"

#include <Psapi.h>

inline LPTHREAD_START_ROUTINE GetLoadLibraryAddress()
{
    HMODULE hMod = GetModuleHandleA("kernel32.dll");
    if (!hMod)
    {
        return nullptr;
    }

    return (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");
}

bool EU4Injector::IsEU4Process(DWORD processId)
{
    HandleWrapper hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    wchar_t procName[MAX_PATH];

    if (hProcess)
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess.get(), &hMod, sizeof(hMod), &cbNeeded))
        {
            if (GetModuleBaseNameW(hProcess.get(), hMod, procName, sizeof(procName) / sizeof(wchar_t)))
            {
                return _wcsicmp(procName, L"eu4.exe");
            }
        }
    }

    return false;
}

bool EU4Injector::Inject(DWORD processId)
{
    wchar_t fullDLLPath[MAX_PATH + 1];
    SIZE_T fullDLLPathLen = GetFullPathNameW(InjectedDLLName, (sizeof(fullDLLPath) - 1) / sizeof(wchar_t), fullDLLPath, nullptr);
    if (fullDLLPathLen == 0)
    {
        return false;
    }

    HandleWrapper hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hProcess)
    {
        LPVOID pathMemory = VirtualAllocEx(hProcess.get(), nullptr, (fullDLLPathLen + 1) * sizeof(wchar_t), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (!pathMemory)
        {
            return false;
        }

        SIZE_T bytesWritten;
        if (!WriteProcessMemory(hProcess.get(), pathMemory, fullDLLPath, (fullDLLPathLen + 1) * sizeof(wchar_t), &bytesWritten))
        {
            return false;
        }

        LPTHREAD_START_ROUTINE loadLibAddress = GetLoadLibraryAddress();
        if (!loadLibAddress)
        {
            return false;
        }

        HandleWrapper hThread = CreateRemoteThread(hProcess.get(), nullptr, 0, loadLibAddress, pathMemory, 0, nullptr);
        if (hThread)
        {
            if (WaitForSingleObject(hThread.get(), 3500) == WAIT_OBJECT_0)
            {
                DWORD exitCode;
                if (GetExitCodeThread(hThread.get(), &exitCode))
                {
                    if (exitCode == 0)
                    {
                        VirtualFreeEx(hProcess.get(), pathMemory, 0, MEM_RELEASE);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else
            {
                return false;
            }
        }

        return false;
    }

    return false;
}

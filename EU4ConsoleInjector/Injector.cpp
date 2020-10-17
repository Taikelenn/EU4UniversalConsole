#include "Injector.h"
#include "HandleWrapper.h"

#include <string>
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
                return _wcsicmp(procName, L"eu4.exe") == 0;
            }
        }
    }

    return false;
}

// https://stackoverflow.com/questions/3828835/how-can-we-check-if-a-file-exists-or-not-using-win32-program
bool FileExists(LPCWSTR szPath)
{
    DWORD dwAttrib = GetFileAttributesW(szPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

// adapted from https://docs.microsoft.com/en-us/windows/win32/psapi/enumerating-all-modules-for-a-process
bool IsAlreadyInjected(HANDLE hProcess)
{
    HMODULE hMods[1024];
    DWORD cbNeeded;

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for (int i = 0; i < cbNeeded / sizeof(HMODULE); i++)
        {
            wchar_t szModName[MAX_PATH];
            if (GetModuleBaseNameW(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
            {
                if (_wcsicmp(szModName, InjectedDLLName) == 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool EU4Injector::Inject(DWORD processId, std::string& msg)
{
    wchar_t fullDLLPath[MAX_PATH + 1];
    memset(fullDLLPath, 0, sizeof(fullDLLPath));

    SIZE_T fullDLLPathLen = GetFullPathNameW(InjectedDLLName, (sizeof(fullDLLPath) - 1) / sizeof(wchar_t), fullDLLPath, nullptr);
    if (fullDLLPathLen == 0)
    {
        msg = "Unexpected error: cannot resolve relative DLL path, error " + std::to_string(GetLastError());
        return false;
    }

    if (!FileExists(fullDLLPath))
    {
        msg = "The file EU4UniversalConsole.dll does not exist. Please ensure that it exists in the same directory as the injector.";
        return false;
    }

    HandleWrapper hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hProcess)
    {
        if (IsAlreadyInjected(hProcess.get()))
        {
            msg = "The console has already been enabled for this game instance.";
            return false;
        }

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
                VirtualFreeEx(hProcess.get(), pathMemory, 0, MEM_RELEASE);

                DWORD exitCode;
                if (GetExitCodeThread(hThread.get(), &exitCode))
                {
                    if (exitCode != 0)
                    {
                        return true;
                    }
                    else
                    {
                        msg = "The console initialization thread failed.";
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

    if (GetLastError() == 5)
    {
        msg = "Cannot access the EU4 process: access is denied.";
    }
    else
    {
        msg = "Cannot access the EU4 process: error " + std::to_string(GetLastError());
    }
    return false;
}

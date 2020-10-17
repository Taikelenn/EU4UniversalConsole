#include <iostream>
#include <string>
#include <vector>

#include <Windows.h>
#include <Psapi.h>

#include "Injector.h"

std::vector<DWORD> GetEU4Processes()
{
	DWORD processIDs[1024];
	DWORD cbNeeded;

	std::vector<DWORD> res;

	if (EnumProcesses(processIDs, sizeof(processIDs), &cbNeeded))
	{
		for (int i = 0; i < cbNeeded / sizeof(DWORD); ++i)
		{
			if (EU4Injector::IsEU4Process(processIDs[i]))
			{
				res.push_back(processIDs[i]);
			}
		}
	}

	return res;
}

int main(int argc, char* argv[])
{
	if (argc >= 2)
	{
		// if command-line arguments are provided, we do not print anything
		bool invalidUsage = argc != 2;

		try
		{
			int pid = std::stoi(argv[1]);
			if (pid <= 0)
			{
				invalidUsage = true;
			}

			if (!invalidUsage)
			{
				// if we provide a PID, we do not check if this is a valid EU4 executable: we trust the user
				std::string injectionMsg;
				return EU4Injector::Inject((DWORD)pid, injectionMsg) ? 0 : 1;
			}
		}
		catch (const std::exception&)
		{
			invalidUsage = true;
		}

		if (invalidUsage)
		{
			std::cout << "Usage: EU4UniversalConsole.exe [EU4.exe PID]" << std::endl;
			return 1;
		}

		return 0;
	}

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO consoleBufferInfo;
	GetConsoleScreenBufferInfo(hStdOut, &consoleBufferInfo);

	WORD prevAttributes = consoleBufferInfo.wAttributes;

	int retval = 0;
	SetConsoleTextAttribute(hStdOut, 15);

	std::vector<DWORD> eu4Processes = GetEU4Processes();
	if (eu4Processes.empty())
	{
		std::cout << "Europa Universalis IV is not running." << std::endl;
	}
	else
	{
		for (DWORD pid : eu4Processes)
		{
			SetConsoleTextAttribute(hStdOut, 15);

			std::cout << "[*] Injecting to Europa Universalis IV, process ID " << pid << std::endl;
			std::string injectionMsg = "";

			if (EU4Injector::Inject(pid, injectionMsg))
			{
				SetConsoleTextAttribute(hStdOut, 10);
				std::cout << " [+] Injection successful! In-game, press INSERT to toggle the console." << std::endl;
			}
			else
			{
				if (!injectionMsg.empty())
				{
					SetConsoleTextAttribute(hStdOut, 14);
					std::cout << " [-] " << injectionMsg << std::endl;
				}

				SetConsoleTextAttribute(hStdOut, 12);
				std::cout << " [-] Injection failed." << std::endl;
			}

			std::cout << std::endl;
		}
	}

	SetConsoleTextAttribute(hStdOut, prevAttributes);
	std::cout << "Press any key to exit." << std::endl;

	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

	INPUT_RECORD ir;
	DWORD readEvents;

	while (ReadConsoleInputA(GetStdHandle(STD_INPUT_HANDLE), &ir, 1, &readEvents))
	{
		if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown)
		{
			break;
		}
	}

	return retval;
}

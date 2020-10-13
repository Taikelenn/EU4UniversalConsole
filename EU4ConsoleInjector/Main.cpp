#include <iostream>
#include <string>

#include "Injector.h"

int main(int argc, char* argv[])
{
	if (argc >= 2)
	{
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
				EU4Injector::Inject((DWORD)pid);
			}
		}
		catch (const std::exception& e)
		{
			invalidUsage = true;
		}

		if (invalidUsage)
		{
			std::cout << L"Usage: EU4UniversalConsole.exe [EU4.exe PID]" << std::endl;
			return 1;
		}

		return 0;
	}
}

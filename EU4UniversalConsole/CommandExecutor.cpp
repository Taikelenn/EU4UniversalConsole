#include "CommandExecutor.h"
#include "CStringArray.h"
#include "DrawingManager.h"
#include "UIConsole.h"

#include "EU4Offsets.h"
#include "SOldCommandData.h"

ULONGLONG CommandExecutor::commandScheduled = false;

CStringArray* SplitString(const std::string& arguments)
{
    std::string currentEntry = "";
    std::vector<CString> splitArgs;

    for (size_t i = 0; i < arguments.size(); ++i)
    {
        if (arguments[i] == ' ')
        {
            if (!currentEntry.empty())
            {
                splitArgs.push_back(currentEntry);
                currentEntry = "";
            }
        }
        else
        {
            currentEntry.push_back(arguments[i]);
        }
    }

    if (!currentEntry.empty())
    {
        splitArgs.push_back(currentEntry);
    }

    return new CStringArray(splitArgs);
}

// EU4 strings use an encoding incompatible with ImGui (Windows-1252?)
// instead of using Unicode like a proper developer should, we'll just switch to nearest ASCII equivalents
// (there's an array called nReplacement, maybe we should use it)
std::string SanitizeString(const std::string& s)
{
    std::string res = "";

    for (size_t i = 0; i < s.size(); ++i)
    {
        unsigned char c = (unsigned char)s[i];
        if (s[i] == 0xfc)
        {
            res.push_back('u');
        }
        else if (c == 0x96)
        {
            res.push_back('-');
        }
        else if (c == 0xe7)
        {
            res.push_back('c');
        }
        else if (c == 0xe9)
        {
            res.push_back('e');
        }
        else if (c == 0xf3)
        {
            res.push_back('o');
        }
        else if (c == 0xfb)
        {
            res.push_back('u');
        }
        else if (c == 0xa3) // some special character? replace it with some nicely-looking ASCII equivalent
        {
            if (s.size() - i >= 4 && s[i + 1] == 'y' && s[i + 2] == 'e' && s[i + 3] == 's')
            {
                res += "[+]";
                i += 3;
            }
            else if (s.size() - i >= 3 && s[i + 1] == 'n' && s[i + 2] == 'o')
            {
                res += "[-]";
                i += 2;
            }
            else
            {
                res.push_back('?');
            }
        }
        else if (c == 0xa7) // a color modifier? should be followed by a color code; skip it entirely
        {
            i += 1;
        }
        else if (s[i] == '\n' || s[i] == '\r' || (s[i] >= 0x20 && s[i] < 0x7f))
        {
            res.push_back(s[i]);
        }
        else
        {
            res.push_back('?');
        }
    }

    return res;
}

CommandFunction_t scheduledCommand_func;
CStringArray* scheduledCommand_args;
UIConsole* scheduledCommand_console;

SOldCommandData* GetCommandDataByName(const std::string& s)
{
    SOldCommandData* commands = EU4Offsets::GetCommandList();

    for (int i = 0; i < EU4Offsets::CommandCount; ++i)
    {
        if (commands[i].commandName == s)
        {
            return &commands[i];
        }
    }

    return nullptr;
}

void CommandExecutor::ExecuteScheduledCommand()
{
    if (commandScheduled)
    {
        commandScheduled |= 1;

        if (DrawingManager::preserveRandomness)
        {
            // TODO: preserve random state
        }

        CCommandResult cmdResult;
        scheduledCommand_func(&cmdResult, scheduledCommand_args);
        if (cmdResult.isSuccessful)
        {
            scheduledCommand_console->AppendEntry(SanitizeString(cmdResult.result.ToString()), ImColor(1.0f, 1.0f, 1.0f));
        }
        else
        {
            scheduledCommand_console->AppendEntry(SanitizeString(cmdResult.result.ToString()), ImColor(1.0f, 0.0f, 0.0f));
        }

        // free allocated memory
        cmdResult.result.Free();
        RemoveScheduledCommand();
    }
}

void CommandExecutor::CheckForCommandTimeout()
{
    // if command is NOT being executed right now (a slight race condition, but we should be fine) and it was more than 2500 ms before the command was scheduled
    if (CommandExecutor::commandScheduled && !(CommandExecutor::commandScheduled & 1) && (GetTickCount64() - CommandExecutor::commandScheduled) > 2500)
    {
        // command execution timeout: unschedule the command and free memory
        UIConsole* uic = scheduledCommand_console;
        if (uic)
        {
            uic->AppendEntry("Command execution timed out. Is a game currently running?", ImColor(1.0f, 1.0f, 0.0f));
        }

        CommandExecutor::RemoveScheduledCommand();
    }
}

void CommandExecutor::RemoveScheduledCommand()
{
    if (scheduledCommand_args)
    {
        scheduledCommand_args->Free();
        delete scheduledCommand_args;
    }

    scheduledCommand_func = nullptr;
    scheduledCommand_args = nullptr;
    scheduledCommand_console = nullptr;

    commandScheduled = 0;
}

void CommandExecutor::ExecuteCommand(const std::string& command, UIConsole* console)
{
    if (commandScheduled)
    {
        console->AppendEntry("Another command is currently queued, please wait a few seconds.", ImColor(1.0f, 1.0f, 1.0f));
        return;
    }

    std::string commandVerb = command;
    std::string argumentString = "";

    size_t firstSpace = commandVerb.find(' ');
    if (firstSpace != std::string::npos)
    {
        commandVerb = command.substr(0, firstSpace);
        argumentString = command.substr(firstSpace);
    }

    SOldCommandData* cmdData = GetCommandDataByName(commandVerb);
    if (cmdData == nullptr)
    {
        console->AppendEntry("This command does not exist", ImColor(1.0f, 1.0f, 0.0f));
    }
    else if (!cmdData->isAvailableInRelease && !DrawingManager::allowDevCommands)
    {
        console->AppendEntry("This command is disallowed in release builds.", ImColor(1.0f, 1.0f, 0.0f));
        console->AppendEntry("You can enable these commands in the console settings menu.", ImColor(1.0f, 1.0f, 0.0f));
    }
    else
    {
        CStringArray* arguments = SplitString(argumentString);

        scheduledCommand_func = cmdData->function;
        scheduledCommand_args = arguments;
        scheduledCommand_console = console;

        commandScheduled = GetTickCount64();
        commandScheduled &= ~(ULONGLONG)1; // clear the LSB
    }
}

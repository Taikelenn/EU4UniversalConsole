#include "CommandExecutor.h"
#include "CStringArray.h"
#include "UIConsole.h"

#include "EU4Offsets.h"

bool CommandExecutor::commandScheduled = false;

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

void CommandExecutor::ExecuteScheduledCommand()
{
    if (CommandExecutor::commandScheduled)
    {
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
        scheduledCommand_args->Free();
        delete scheduledCommand_args;

        scheduledCommand_func = nullptr;
        scheduledCommand_args = nullptr;
        scheduledCommand_console = nullptr;

        CommandExecutor::commandScheduled = false;
    }
}

void CommandExecutor::ExecuteCommand(const std::string& command, UIConsole* console)
{
    std::string commandVerb = command;
    std::string argumentString = "";

    size_t firstSpace = commandVerb.find(' ');
    if (firstSpace != std::string::npos)
    {
        commandVerb = command.substr(0, firstSpace);
        argumentString = command.substr(firstSpace);
    }

    CStringArray* arguments = SplitString(argumentString);

    // temporarily, support only the "event" command for testing
    if (commandVerb == "event")
    {
        std::ptrdiff_t eventFunc = 0xbca9f0; // this offset is not in the EU4Offsets namespace because it is temporary and will be removed

        scheduledCommand_func = (CommandFunction_t)EU4Offsets::TranslateOffset(eventFunc);
        scheduledCommand_args = arguments;
        scheduledCommand_console = console;

        commandScheduled = true;
    }
    else
    {
        console->AppendEntry("This command does not exist", ImColor(1.0f, 1.0f, 0.0f));
    }
}

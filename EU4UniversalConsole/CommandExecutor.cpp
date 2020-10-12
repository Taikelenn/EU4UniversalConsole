#include "CommandExecutor.h"
#include "CStringArray.h"

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

CommandFunction_t scheduledCommand_func;
CStringArray* scheduledCommand_args;

void CommandExecutor::ExecuteScheduledCommand()
{
    if (CommandExecutor::commandScheduled)
    {
        CCommandResult cmdResult;
        CCommandResult* cmdResult2 = scheduledCommand_func(&cmdResult, scheduledCommand_args);

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
        commandScheduled = true;
    }
    else
    {
        
    }
}

#include "CommandExecutor.h"
#include "CStringArray.h"

#include "EU4Offsets.h"

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

bool ExecuteCommandInternal(const std::string& command, std::string& resultMsg)
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

        CommandFunction_t cmd = (CommandFunction_t)EU4Offsets::TranslateOffset(eventFunc);
        CCommandResult cmdResult;
        CCommandResult* cmdResult2 = cmd(&cmdResult, arguments);
    }
    else
    {
        resultMsg = "Command not recognized.";
    }

    return false;
}

void CommandExecutor::ExecuteCommand(const std::string& command, UIConsole* console)
{
    std::string result;
    if (ExecuteCommandInternal(command, result))
    {
        // append the result
    }
    else
    {
        // append the result as an error
    }
}

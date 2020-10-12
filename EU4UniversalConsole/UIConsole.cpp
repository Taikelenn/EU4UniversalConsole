#include "UIConsole.h"

#include <cstring>

UIConsole::UIConsole()
{
    memset(commandBuffer, 0, sizeof(commandBuffer));
    historyPosition = -1;
}

int UIConsole::InputCallbackInternal(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
	{
        // if the history is empty, there's nothing to show
        if (historicalEntries.empty())
        {
            historyPosition = -1;
            return 0;
        }

        int prevHistoryPosition = historyPosition;

        if (data->EventKey == ImGuiKey_UpArrow)
        {
            if (historyPosition == -1)
            {
                historyPosition = (int)historicalEntries.size() - 1;
            }
            else
            {
                --historyPosition;
                if (historyPosition == -1)
                {
                    historyPosition = 0;
                }
            }
        }
        else if (data->EventKey == ImGuiKey_DownArrow)
        {
            if (historyPosition != -1)
            {
                ++historyPosition;
                if (historyPosition >= (int)historicalEntries.size())
                {
                    historyPosition = -1;
                }
            }
        }

        if (prevHistoryPosition != historyPosition)
        {
            data->DeleteChars(0, data->BufTextLen);

            if (historyPosition != -1)
            {
                data->InsertChars(0, historicalEntries[historyPosition].c_str());
            }
        }
	}

	return 0;
}

void UIConsole::Render()
{
    bool reclaimFocus = false;
    
	if (ImGui::InputText("Input", commandBuffer, IM_ARRAYSIZE(commandBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory, UIConsole::InputCallback, this))
	{
        historicalEntries.push_back(commandBuffer);
        commandBuffer[0] = 0;
        historyPosition = -1;

        reclaimFocus = true;
	}

    ImGui::SetItemDefaultFocus();
    if (reclaimFocus)
    {
        ImGui::SetKeyboardFocusHere(-1);
    }
}

int UIConsole::InputCallback(ImGuiInputTextCallbackData* data)
{
	return static_cast<UIConsole*>(data->UserData)->InputCallbackInternal(data);
}

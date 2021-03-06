#include "UIConsole.h"
#include "CommandExecutor.h"

#include <cstring>

UIConsole::UIConsole()
{
	this->Clear();
}

void UIConsole::ExecuteCommand(const std::string& command)
{
	CommandExecutor::ExecuteCommand(command, this);
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
	const float consoleFooterHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

	ImGui::BeginChild("UICScrollingRegion", ImVec2(0, -consoleFooterHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

	for (size_t i = 0; i < consoleData.size(); i++)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)consoleData[i].second);
		ImGui::TextUnformatted(consoleData[i].first.c_str());
		ImGui::PopStyleColor();
	}

	if (shouldScrollDown)
	{
		ImGui::SetScrollHereY(1.0f);
		shouldScrollDown = false;
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();

	ImGui::Separator();

	if (ImGui::InputText("Input", commandBuffer, IM_ARRAYSIZE(commandBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory, UIConsole::InputCallback, this))
	{
		// if the command was not empty, execute it
		if (commandBuffer[0])
		{
			std::string cmdString = commandBuffer;

			historicalEntries.push_back(cmdString);

			this->AppendEntry("# " + cmdString, ImColor(0.0f, 1.0f, 1.0f));
			this->ExecuteCommand(cmdString);
		}

		// clear the command input box and reset history state
		commandBuffer[0] = 0;
		historyPosition = -1;

		reclaimFocus = true;
	}

	ImGui::SetItemDefaultFocus();
	if (reclaimFocus)
	{
		ImGui::SetKeyboardFocusHere(-1);
	}

	reclaimFocus = false;
}

void UIConsole::AppendEntry(const std::string& text, ImColor color)
{
	consoleData.push_back(std::make_pair(text, color));
	shouldScrollDown = true;
}

void UIConsole::Clear()
{
	consoleData.clear();
	historicalEntries.clear();
	historyPosition = -1;
	shouldScrollDown = false;
	reclaimFocus = true;

	memset(commandBuffer, 0, sizeof(commandBuffer));

	this->AppendEntry("Welcome to EU4UniversalConsole!", ImColor(0.9f, 0.7f, 0.9f));
}

int UIConsole::InputCallback(ImGuiInputTextCallbackData* data)
{
	return static_cast<UIConsole*>(data->UserData)->InputCallbackInternal(data);
}

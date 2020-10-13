#include "DrawingManager.h"
#include "HookManager.h"

#include "UIConsole.h"
#include "EU4Offsets.h"
#include "SOldCommandData.h"
#include "CommandExecutor.h"

#include "imgui/imgui.h"

#include <algorithm>

void DrawingManager::RenderOverlay()
{
	ImGui::SetNextWindowSize(ImVec2(750, 600), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Universal console", nullptr, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Settings"))
			{
				ImGui::MenuItem("Intercept all input", nullptr, &DrawingManager::inputBlocked);
				ImGui::MenuItem("Allow developer commands", nullptr, &DrawingManager::allowDevCommands);
				ImGui::MenuItem("Inspect entire command list", nullptr, &DrawingManager::listingCommands);
				if (ImGui::MenuItem("Unload"))
				{
					SetEvent(HookManager::hUnloadEvent);
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::Text("Welcome to EU4UniversalConsole, built on %s", __DATE__ " " __TIME__);

		if (ImGui::CollapsingHeader("Console"))
		{
			static UIConsole console;
			console.Render();
		}
	}
	ImGui::End();

	if (listingCommands)
	{
		static int selectedCommand = -1;
		static char filterBuf[256];

		ImGui::SetNextWindowSize(ImVec2(700, 450), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Command list", &listingCommands))
		{
			static std::vector<SOldCommandData> commandsInfo;
			if (commandsInfo.empty())
			{
				commandsInfo.reserve(EU4Offsets::CommandCount);
				commandsInfo.insert(commandsInfo.end(), EU4Offsets::GetCommandList(), EU4Offsets::GetCommandList() + EU4Offsets::CommandCount);
				std::sort(commandsInfo.begin(), commandsInfo.end(), [](const SOldCommandData& a, const SOldCommandData& b) { return strcmp(a.commandName, b.commandName) < 0; });

				for (size_t i = 0; i < commandsInfo.size(); ++i)
				{
					// prevent potential crashes by avoiding nullptrs
					if (commandsInfo[i].commandDescription == nullptr)
					{
						commandsInfo[i].commandDescription = "";
					}
				}
			}

			const float descHeight = 100;
			const float listFooterHeight = 2 * ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() + descHeight;
			ImGui::BeginChild("UICommandList", ImVec2(0, -listFooterHeight));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

			for (size_t i = 0; i < commandsInfo.size(); ++i)
			{
				if (*filterBuf && strstr(commandsInfo[i].commandName, filterBuf) == nullptr)
				{
					continue;
				}

				if (ImGui::Selectable(commandsInfo[i].commandName, selectedCommand == i))
				{
					selectedCommand = (int)i;
				}
			}

			ImGui::PopStyleVar();
			ImGui::EndChild();

			ImGui::Separator();
			ImGui::InputTextWithHint("Command filter", "enter a substring here", filterBuf, sizeof(filterBuf));

			ImGui::BeginChild("UICommandListDesc", ImVec2(0, descHeight), true);

			if (selectedCommand == -1)
			{
				ImGui::TextColored((ImVec4)ImColor(0.0f, 1.0f, 1.0f), "%s", "Select a command to view detailed information.");
			}
			else
			{
				ImGui::PushTextWrapPos();

				ImGui::TextUnformatted("Description:");
				ImGui::SameLine();
				ImGui::TextUnformatted(commandsInfo[selectedCommand].commandDescription);

				std::string usageStr = commandsInfo[selectedCommand].commandName;
				for (uint64_t i = 0; i < commandsInfo[selectedCommand].argumentCount; ++i)
				{
					const char* argVal = commandsInfo[selectedCommand].argumentDescription[i];

					usageStr += " ";
					usageStr += argVal ? argVal : "<???>";
				}

				ImGui::TextUnformatted("Usage:");
				ImGui::SameLine();
				ImGui::TextUnformatted(usageStr.c_str());

				if (!commandsInfo[selectedCommand].isAvailableInRelease)
				{
					ImGui::NewLine();
					ImGui::TextColored((ImVec4)ImColor(1.0f, 1.0f, 0.0f), "%s", "Warning: this command is by default unavailable in EU4 release build. Execute at your own risk!");
				}

				ImGui::PopTextWrapPos();
			}

			ImGui::EndChild();
		}
		ImGui::End();

		// the window was just closed
		if (!listingCommands)
		{
			*filterBuf = 0;
			selectedCommand = -1;
		}
	}
}

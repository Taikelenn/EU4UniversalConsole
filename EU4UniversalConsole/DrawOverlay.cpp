#include "DrawingManager.h"
#include "HookManager.h"

#include "UIConsole.h"
#include "EU4Offsets.h"
#include "SOldCommandData.h"

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
		ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Command list", &listingCommands))
		{
			static std::vector<SOldCommandData> commandsInfo;
			if (commandsInfo.empty())
			{
				commandsInfo.reserve(EU4Offsets::CommandCount);
				commandsInfo.insert(commandsInfo.end(), EU4Offsets::GetCommandList(), EU4Offsets::GetCommandList() + EU4Offsets::CommandCount);
				std::sort(commandsInfo.begin(), commandsInfo.end(), [](const SOldCommandData& a, const SOldCommandData& b) { return strcmp(a.commandName, b.commandName) < 0; });
			}

			const float listFooterHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
			ImGui::BeginChild("UICommandList", ImVec2(0, -listFooterHeight));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

			static int selectedCommand = -1;
			static char filterBuf[256];

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
		}
		ImGui::End();
	}
}

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

		SOldCommandData* cmd = EU4Offsets::GetCommandList();
		for (int i = 0; i < 10; ++i)
		{
			ImGui::Text("%s", cmd[i].commandName);
		}
	}
	ImGui::End();

	if (listingCommands)
	{
		if (ImGui::Begin("Command list", &listingCommands))
		{
			static std::vector<SOldCommandData> commandsInfo;
			if (commandsInfo.empty())
			{
				commandsInfo.reserve(EU4Offsets::CommandCount);
				commandsInfo.insert(commandsInfo.end(), EU4Offsets::GetCommandList(), EU4Offsets::GetCommandList() + EU4Offsets::CommandCount);
				std::sort(commandsInfo.begin(), commandsInfo.end(), [](const SOldCommandData& a, const SOldCommandData& b) { return strcmp(a.commandName, b.commandName) < 0; });
			}

			for (const auto& cmd : commandsInfo)
			{
				ImGui::Text(cmd.commandName);
			}
		}
		ImGui::End();
	}
}

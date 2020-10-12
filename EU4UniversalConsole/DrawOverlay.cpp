#include "DrawingManager.h"
#include "HookManager.h"

#include "UIConsole.h"

#include "imgui/imgui.h"

void DrawingManager::RenderOverlay()
{
	ImGui::SetNextWindowSize(ImVec2(750, 800), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("EU4UniversalConsole", nullptr, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Settings"))
			{
				ImGui::MenuItem("Intercept all input", NULL, &DrawingManager::inputBlocked);
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
}

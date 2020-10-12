#include "DrawingManager.h"
#include "HookManager.h"

#include "UIConsole.h"

#include "imgui/imgui.h"

void DrawingManager::RenderOverlay()
{
	if (ImGui::Begin("EU4UniversalConsole"))
	{
		ImGui::Text("Welcome to EU4UniversalConsole, built on %s", __DATE__ " " __TIME__);

		if (ImGui::CollapsingHeader("Console"))
		{
			static UIConsole console;
			console.Render();

			
		}

		if (ImGui::Button("Unload"))
		{
			SetEvent(HookManager::hUnloadEvent);
		}
	}
	ImGui::End();
}

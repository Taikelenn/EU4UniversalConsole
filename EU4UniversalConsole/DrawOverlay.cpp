#include "DrawingManager.h"
#include "HookManager.h"

#include "imgui/imgui.h"

void DrawingManager::RenderOverlay()
{
	if (ImGui::Begin("EU4UniversalConsole"))
	{
		ImGui::Text("Welcome to EU4UniversalConsole, built on %s", __DATE__ " " __TIME__);
		ImGui::Spacing();

		if (ImGui::Button("Unload"))
		{
			SetEvent(HookManager::hUnloadEvent);
		}
	}
	ImGui::End();
}

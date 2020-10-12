#include "DrawingManager.h"

#include "imgui/imgui.h"

void DrawingManager::RenderOverlay()
{
	if (ImGui::Begin("EU4UniversalConsole"))
	{
		ImGui::Text("Welcome to EU4UniversalConsole, built on %s", __DATE__ " " __TIME__);
		ImGui::Spacing();
	}
	ImGui::End();
}

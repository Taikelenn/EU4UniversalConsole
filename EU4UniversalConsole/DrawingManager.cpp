#include "DrawingManager.h"
#include "EU4Offsets.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"

static bool menuEnabled;

void DrawingManager::Initialize(IDirect3DDevice9* device)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NoMouseCursorChange;

	ImGui_ImplWin32_Init(EU4Offsets::GetWindowHandle());
	ImGui_ImplDX9_Init(device);

	ImGui::StyleColorsDark();
}

void DrawingManager::RenderFrame(IDirect3DDevice9* device)
{
	if (!device || !menuEnabled)
	{
		return;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
	RenderOverlay();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool DrawingManager::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lResultPtr)
{
	// toggle menu on the INSERT key press
	if (uMsg == WM_KEYDOWN && wParam == VK_INSERT)
	{
		menuEnabled = !menuEnabled;
	}

	// handle ImGui events and suppress them if they were handled successfully
	LRESULT imGuiResult = ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
	if (imGuiResult)
	{
		*lResultPtr = imGuiResult;
		return true;
	}

	return false;
}

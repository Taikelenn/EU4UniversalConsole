#include "DrawingManager.h"
#include "EU4Offsets.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"

static bool menuEnabled;
static bool isDestroying;
static LARGE_INTEGER perfFreq;

bool DrawingManager::inputBlocked = true;
bool DrawingManager::listingCommands = false;
bool DrawingManager::allowDevCommands = false;
bool DrawingManager::preserveRandomness = true;

unsigned long long DrawingManager::lastFrameHookTime;

void DrawingManager::Initialize(IDirect3DDevice9* device)
{
	QueryPerformanceFrequency(&perfFreq);
	perfFreq.QuadPart /= 1000;

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NoMouseCursorChange;
	io.IniFilename = nullptr; // don't use the imgui.ini file

	ImGui_ImplWin32_Init(EU4Offsets::GetWindowHandle());
	ImGui_ImplDX9_Init(device);

	ImGui::StyleColorsDark();
}

void DrawingManager::RenderFrame(IDirect3DDevice9* device)
{
	if (!device || !menuEnabled || isDestroying)
	{
		return;
	}

	LARGE_INTEGER beginTick, endTick;
	QueryPerformanceCounter(&beginTick);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	RenderOverlay();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	QueryPerformanceCounter(&endTick);
	DrawingManager::lastFrameHookTime = (endTick.QuadPart - beginTick.QuadPart) / perfFreq.QuadPart;
}

// this function must NOT be called from a drawing thread
void DrawingManager::Destroy()
{
	menuEnabled = false;
	isDestroying = true;

	Sleep(50); // wait a tiny amount of time to make sure that no drawing occurs

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool DrawingManager::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lResultPtr)
{
	// toggle menu on the INSERT key press
	if (uMsg == WM_KEYDOWN && wParam == VK_INSERT && !isDestroying)
	{
		menuEnabled = !menuEnabled;
	}

	if (menuEnabled)
	{
		// handle ImGui events and suppress them if they were handled successfully
		LRESULT imGuiResult = ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		if (imGuiResult)
		{
			*lResultPtr = imGuiResult;
			return true;
		}

		// block game input while in menu
		if (DrawingManager::inputBlocked && (uMsg == WM_CHAR || uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP || uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEMOVE))
		{
			*lResultPtr = 0;
			return true;
		}
	}

	return false;
}

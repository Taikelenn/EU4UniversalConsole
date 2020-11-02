#pragma once

#include <string>
#include <vector>

#include "imgui/imgui.h"

// This class is largely based on the example console widget found in imgui_demo.cpp
class UIConsole {
private:
	char commandBuffer[2048];
	std::vector<std::string> historicalEntries;
	int historyPosition;
	bool shouldScrollDown;
	bool reclaimFocus;

	std::vector<std::pair<std::string, ImColor>> consoleData;

	void ExecuteCommand(const std::string& command);
	int InputCallbackInternal(ImGuiInputTextCallbackData* data);
public:
	UIConsole();

	void Render();
	void AppendEntry(const std::string& text, ImColor color);

	void Clear();

	static int InputCallback(ImGuiInputTextCallbackData* data);
};

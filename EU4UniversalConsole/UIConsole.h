#pragma once

#include <string>
#include <vector>

#include "imgui/imgui.h"

class UIConsole {
private:
	char commandBuffer[2048];
	std::vector<std::string> historicalEntries;
	int historyPosition;

	int InputCallbackInternal(ImGuiInputTextCallbackData* data);

public:
	UIConsole();

	void Render();
	static int InputCallback(ImGuiInputTextCallbackData* data);
};

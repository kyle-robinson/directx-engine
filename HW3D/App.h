#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImGuiManager.h"

class App
{
public:
	App();
	~App();
	int Init();
private:
	void DoFrame();
private:
	ImGuiManager imgui;
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
	float speed_factor = 1.0f;
};
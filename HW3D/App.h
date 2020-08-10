#pragma once
#include "Window.h"
#include "Timer.h"

class App
{
public:
	App();
	~App();
	int Init();
private:
	void DoFrame();
private:
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<class Box>> boxes;
};
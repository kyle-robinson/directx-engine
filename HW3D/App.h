#pragma once
#include "Window.h"
#include "Timer.h"

class App
{
public:
	App();
	int Init();
private:
	void Update();
private:
	Window wnd;
	Timer timer;
};
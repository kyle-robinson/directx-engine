#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "PointLight.h"
#include "ImGuiManager.h"
#include "Model.h"

class App
{
public:
	App();
	~App();
	int Init();
private:
	void DoFrame();
	void ShowModelWindow();
private:
	ImGuiManager imgui;
	Window wnd;
	Timer timer;
	Camera camera;
	PointLight light;

	float speed_factor = 1.0f;
	Model nanosuit{ wnd.Gfx(), "res\\models\\nanosuit.obj" };

	struct
	{
		float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
		float x = 0.0f, y = 0.0f, z = 0.0f;
	} pos;
};
#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "PointLight.h"
#include "ImGuiManager.h"
#include "Mesh.h"

class App
{
public:
	App();
	~App();
	int Init();
private:
	void DoFrame();
	void ShowRawInputWindow();
private:
	ImGuiManager imgui;
	Window wnd;
	Timer timer;
	Camera camera;
	PointLight light;

	float speed_factor = 1.0f;
	Model nanosuit{ wnd.Gfx(), "res\\models\\nanosuit.gltf" };
	
	int x = 0, y = 0;
	bool cursorEnabled = true;
};
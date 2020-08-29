#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "PointLight.h"
#include "ImGuiManager.h"
#include "Mesh.h"
#include "NormalPlane.h"
#include "NormalCube.h"
#include "Stencil.h"

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

	Model sponza{ wnd.Gfx(), "res\\models\\sponza\\sponza.obj", 1.0f / 20.0f };
	NormalCube cube{ wnd.Gfx(), 4.0f };
	NormalCube cube2{ wnd.Gfx(), 4.0f };
	//Model goblin{ wnd.Gfx(), "res\\models\\goblin\\GoblinX.obj", 6.0f };
	//Model nanosuit{ wnd.Gfx(), "res\\models\\nanosuit\\nanosuit.obj", 2.0f };
	//Model wall{ wnd.Gfx(), "res\\models\\brick_wall\\brick_wall.obj", 6.0f };
	//NormalPlane plane{ wnd.Gfx(), 6.0f };
	
	std::string commandLine;
	int x = 0, y = 0;
};
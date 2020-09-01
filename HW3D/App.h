#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "PointLight.h"
#include "ImGuiManager.h"
#include "Model.h"
#include "NormalCube.h"
#include "FrameCommander.h"
#include "Material.h"

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
	FrameCommander fc;

	Model sponza{ wnd.Gfx(), "res\\models\\sponza\\sponza.obj", 1.0f / 20.0f };
	//Model goblin{ wnd.Gfx(), "res\\models\\goblin\\GoblinX.obj", 4.0f };
	//Model backpack{ wnd.Gfx(), "res\\models\\backpack\\backpack.obj", 4.0f };
	NormalCube cube{ wnd.Gfx(), 4.0f };
	NormalCube cube2{ wnd.Gfx(), 4.0f };
	
	//std::unique_ptr<Mesh> pLoaded;
	std::string commandLine;
	int x = 0, y = 0;
};
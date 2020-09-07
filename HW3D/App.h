#pragma once
#include "Math.h"
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "PointLight.h"
#include "ImGuiManager.h"
#include "Model.h"
#include "NormalCube.h"
#include "BlurOutlineRG.h"

class App
{
public:
	App();
	~App();
	int Init();
private:
	void DoFrame( float dt );
	void HandleInput( float dt );
	void ShowRawInputWindow();
private:
	ImGuiManager imgui;
	Window wnd;
	Timer timer;
	Camera camera{ { -13.f, 6.0f, 3.5f }, 0.0f, PI / 2.0f };
	PointLight light;
	Rgph::BlurOutlineRG rg{ wnd.Gfx() };

	Model sponza{ wnd.Gfx(), "res\\models\\sponza\\sponza.obj", 1.0f / 20.0f };
	Model nanosuit{ wnd.Gfx(), "res\\models\\nanosuit\\nanosuit.obj", 4.0f };
	Model goblin{ wnd.Gfx(), "res\\models\\goblin\\GoblinX.obj", 4.0f };
	Model backpack{ wnd.Gfx(), "res\\models\\backpack\\backpack.obj", 4.0f };
	NormalCube cube{ wnd.Gfx(), 4.0f };
	NormalCube cube2{ wnd.Gfx(), 4.0f };

	std::string commandLine;
	int x = 0, y = 0;
};
#pragma once
#include "Math.h"
#include "Window.h"
#include "Timer.h"
#include "CameraContainer.h"
#include "PointLight.h"
#include "ImGuiManager.h"
#include "Model.h"
#include "NormalCube.h"
#include "BlurOutlineRG.h"
#include "ScriptCommander.h"

class App
{
public:
	App( const std::string& commandLine );
	~App();
	int Init();
private:
	void DoFrame( float dt );
	void HandleInput( float dt );
	void ShowRawInputWindow();
private:
	ImGuiManager imgui;
	ScriptCommander scriptCommander;
	Window wnd;
	Timer timer;
	CameraContainer cameras;
	PointLight light;
	PointLight light1;
	PointLight light2;
	PointLight light3;
	PointLight light4;
	Rgph::BlurOutlineRG rg{ wnd.Gfx() };

	Model sponza{ wnd.Gfx(), "res\\models\\sponza\\sponza.obj", 1.0f / 20.0f };
	Model nanosuit{ wnd.Gfx(), "res\\models\\nanosuit\\nanosuit.obj", 2.0f };
	Model goblin{ wnd.Gfx(), "res\\models\\goblin\\GoblinX.obj", 4.0f };
	Model backpack{ wnd.Gfx(), "res\\models\\backpack\\backpack.obj", 4.0f };
	NormalCube cube{ wnd.Gfx(), 4.0f };
	NormalCube cube2{ wnd.Gfx(), 4.0f };

	std::string commandLine;
	int x = 0, y = 0;
	bool saveDepth = false;

	bool loadSponza = false;
	bool loadNanosuit = false;
	bool loadGoblin = false;
	bool loadBackpack = false;

	bool loadCameras = false;
	bool loadCube1 = false;
	bool loadCube2 = false;

	bool loadLight = false;
	bool loadLight1 = false;
	bool loadLight2 = false;
	bool loadLight3 = false;
	bool loadLight4 = false;

	bool loadShadow = false;
	bool loadBlur = false;
	bool loadRaw = false;
};
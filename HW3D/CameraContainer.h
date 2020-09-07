#pragma once
#include <vector>
#include <memory>

class Graphics;
class Camera;

class CameraContainer
{
public:
	void SpawnControlWindow();
	void Bind( Graphics& gfx );
	void AddCamera( std::unique_ptr<Camera> pCam );
	Camera& GetCamera();
	~CameraContainer();
private:
	std::vector<std::unique_ptr<Camera>> cameras;
	int selected = 0;
};
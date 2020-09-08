#pragma once
#include <vector>
#include <memory>

class Graphics;
class Camera;

namespace Rgph
{
	class RenderGraph;
}

class CameraContainer
{
public:
	void SpawnControlWindow( Graphics& gfx );
	void Bind( Graphics& gfx );
	void AddCamera( std::unique_ptr<Camera> pCam );
	Camera* operator->();
	~CameraContainer();
	void LinkTechniques( Rgph::RenderGraph& rg );
	void Submit() const;
private:
	Camera& GetControlledCamera();
private:
	std::vector<std::unique_ptr<Camera>> cameras;
	int active = 0, controlled = 0;
};
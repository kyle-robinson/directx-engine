#pragma once
#include "Projection.h"
#include "CameraIndicator.h"
#include <string>

class Graphics;

namespace Rgph
{
	class RenderGraph;
}

class Camera
{
public:
	Camera( Graphics& gfx, std::string name, DirectX::XMFLOAT3 initialPos = { 0.0f, 0.0f, 0.0f }, float initialPitch = 0.0f, float initialYaw = 0.0f ) noexcept;
	void BindToGraphics( Graphics& gfx ) const;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWidgets() noexcept;
	void Reset() noexcept;
	void Rotate( float dx, float dy ) noexcept;
	void Translate( DirectX::XMFLOAT3 translation ) noexcept;
	const std::string& GetName() const noexcept;
	void LinkTechniques( Rgph::RenderGraph& rg );
	void Submit() const;
private:
	std::string name;
	DirectX::XMFLOAT3 initialPos;
	float initialPitch, initialYaw;
	DirectX::XMFLOAT3 pos;
	float pitch, yaw;
	float travelSpeed = 12.0f, rotationSpeed = 0.002f;
	Projection proj;
	CameraIndicator indicator;
};
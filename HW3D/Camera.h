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
private:
	enum Param
	{
		Position,
		Rotation,
		Speed,
		Indicator
	};
public:
	Camera( Graphics& gfx,
		std::string name,
		DirectX::XMFLOAT3 initialPos = { 0.0f, 0.0f, 0.0f },
		float initialPitch = 0.0f,
		float initialYaw = 0.0f,
		float initialTravSpeed = 12.0f,
		float initialRotSpeed = 0.002f,
		bool tethered = false ) noexcept;
	void BindToGraphics( Graphics& gfx ) const;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SpawnControlWidgets( Graphics& gfx ) noexcept;
	void Reset( Graphics& gfx, Param param ) noexcept;
	void Rotate( float dx, float dy ) noexcept;
	void Translate( DirectX::XMFLOAT3 translation ) noexcept;
	DirectX::XMFLOAT3 GetPosition() const noexcept;
	void SetPosition( const DirectX::XMFLOAT3& pos ) noexcept;
	const std::string& GetName() const noexcept;
	void LinkTechniques( Rgph::RenderGraph& rg );
	void Submit( size_t channel ) const;
private:
	bool tethered;

	std::string name;
	DirectX::XMFLOAT3 initialPos;
	DirectX::XMFLOAT3 pos;

	float initialPitch, initialYaw;
	float pitch, yaw;

	float initialTravSpeed, initialRotSpeed;
	float travelSpeed, rotationSpeed;

	bool enableCameraIndicator = true;
	bool enableFrustumIndicator = true;
	
	Projection proj;
	CameraIndicator indicator;
};
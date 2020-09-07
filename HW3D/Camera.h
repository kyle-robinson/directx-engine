#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera( std::string name, DirectX::XMFLOAT3 initialPos = { 0.0f, 0.0f, 0.0f }, float initialPitch = 0.0f, float initialYaw = 0.0f ) noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWidgets() noexcept;
	void Reset() noexcept;
	void Rotate( float dx, float dy ) noexcept;
	void Translate( DirectX::XMFLOAT3 translation ) noexcept;
	const std::string& GetName() const noexcept;
private:
	std::string name;
	DirectX::XMFLOAT3 initialPos;
	float initialPitch, initialYaw;
	DirectX::XMFLOAT3 pos;
	float pitch, yaw;
	float travelSpeed = 12.0f, rotationSpeed = 0.002f;
};
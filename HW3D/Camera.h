#pragma once
#include "Graphics.h"

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
private:
	float r = 20.0f, theta = 0.0f, phi = 0.0f;
	float pitch = 0.0f, yaw = 0.0, roll = 0.0f;
};
#pragma once
#include "DrawableBase.h"

class Box : public DrawableBase<Box>
{
public:
	Box( Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 material );
	void Update( float dt ) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	// positional
	float r;
	float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
	float theta, phi, chi;
	// speed (delta/s)
	float droll, dpitch, dyaw;
	float dtheta, dphi, dchi;
	// model transform
	DirectX::XMFLOAT3X3 mt;
};
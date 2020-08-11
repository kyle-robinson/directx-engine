#pragma once
#include "DrawableBase.h"

class Sheet : public DrawableBase<Sheet>
{
public:
	Sheet( Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist );
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
};
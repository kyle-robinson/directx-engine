#pragma once
#include "DrawableBase.h"

template<class T>
class PrimitiveObject : public DrawableBase<T>
{
public:
	PrimitiveObject(Graphics& gfx, std::mt19937 rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist)
		:
		r(rdist(rng)),
		droll(ddist(rng)),
		dpitch(ddist(rng)),
		dyaw(ddist(rng)),
		dtheta(odist(rng)),
		dphi(odist(rng)),
		dchi(odist(rng)),
		theta(rdist(rng)),
		phi(rdist(rng)),
		chi(rdist(rng))
	{}
	void Update( float dt ) noexcept
	{
		roll += droll * dt;
		pitch += dpitch * dt;
		yaw += dyaw * dt;
		theta += dtheta * dt;
		phi += dphi * dt;
		chi += dchi * dt;
	}
	DirectX::XMMATRIX GetTransformXM() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw( pitch, yaw, roll ) *
			DirectX::XMMatrixTranslation( r, 0.0f, 0.0f ) *
			DirectX::XMMatrixRotationRollPitchYaw( theta, phi, chi );
	}
private:
	// positional
	float r;
	float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
	float theta, phi, chi;
	// speed (delta/s)
	float droll, dpitch, dyaw;
	float dtheta, dphi, dchi;
};
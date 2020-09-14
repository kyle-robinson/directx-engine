#pragma once
#include "Drawable.h"

class SolidSphere : public Drawable
{
public:
	SolidSphere( Graphics& gfx, float radius );
	void SetPosition( DirectX::XMFLOAT3 pos ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControls() noexcept;
private:
	DirectX::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
};
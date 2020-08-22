#pragma once
#include "Drawable.h"

class NormalCube : public Drawable
{
public:
	NormalCube( Graphics& gfx, float size );
	void SetPos( DirectX::XMFLOAT3 pos ) noexcept;
	void SetRotation( float roll, float pitch, float yaw ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow( Graphics& gfx ) noexcept;
private:
	struct PSMaterialConstant
	{
		float specularIntensity = 0.1f;
		float specularPower = 20.0f;
		BOOL normalMapEnabled = TRUE;
		float padding[1];
	} pmc;
	DirectX::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
	float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
};
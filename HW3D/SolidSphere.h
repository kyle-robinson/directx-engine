#pragma once
#include "Drawable.h"

class SolidSphere : public Drawable
{
public:
	struct PSColorConstant
	{
		DirectX::XMFLOAT3 color;
		float padding;
	} colorConst;
	SolidSphere( Graphics& gfx, float radius );
	void SetPosition( DirectX::XMFLOAT3 pos ) noexcept;
	void SetColor( DirectX::XMFLOAT3 color ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
};
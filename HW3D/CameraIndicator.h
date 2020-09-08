#pragma once
#include "Drawable.h"

class CameraIndicator : public Drawable
{
public:
	CameraIndicator( Graphics& gfx );
	void SetPosition( DirectX::XMFLOAT3 pos ) noexcept;
	void SetRotation( DirectX::XMFLOAT3 rot ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 rot = { 0.0f, 0.0f, 0.0f };
};
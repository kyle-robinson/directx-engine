#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"

class PointLight
{
public:
	PointLight( Graphics& gfx, float radius = 0.5f );
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Draw( Graphics& gfx ) const noexcept(!IS_DEBUG);
	void Bind( Graphics& gfx ) const noexcept;
private:
	struct PointLightCBuf
	{
		struct
		{
			DirectX::XMFLOAT3 pos;
			float padding;
		};
		struct
		{
			DirectX::XMFLOAT3 color;
			float padding;
		};
	};
private:
	DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 color = { 1.0f, 0.0f, 0.5f };
	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};
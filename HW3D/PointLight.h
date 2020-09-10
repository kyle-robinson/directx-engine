#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"

class Camera;

namespace Rgph
{
	class RenderGraph;
}

class PointLight
{
public:
	PointLight( Graphics& gfx, float radius = 0.5f );
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Submit() const noexcept(!IS_DEBUG);
	void Bind( Graphics& gfx, DirectX::FXMMATRIX view ) const noexcept;
	void LinkTechniques( Rgph::RenderGraph& );
	std::shared_ptr<Camera> ShareCamera() const noexcept;
private:
	struct PointLightCBuf
	{
		alignas( 16 ) DirectX::XMFLOAT3 lightPos;
		alignas( 16 ) DirectX::XMFLOAT3 ambient;
		alignas( 16 ) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
private:
	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	std::shared_ptr<Camera> pCamera;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> cbuf;
};
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
	PointLight( Graphics& gfx, DirectX::XMFLOAT3 pos = { 10.0f, 9.0f, 2.5f }, float radius = 0.5f );
	void SpawnControlWindow( const char* name ) noexcept;
	void Reset() noexcept;
	void Submit( size_t channels ) const noexcept(!IS_DEBUG);
	void Bind( Graphics& gfx, DirectX::FXMMATRIX view ) const noexcept;
	void LinkTechniques( Rgph::RenderGraph& );
	std::shared_ptr<Camera> ShareCamera() const noexcept;
	mutable SolidSphere mesh;
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
	PointLightCBuf initial;
	PointLightCBuf cbData;
	std::shared_ptr<Camera> pCamera;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> cbuf;
};
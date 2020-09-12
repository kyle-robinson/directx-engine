#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

class Camera;

namespace Bind
{
	class SkyboxTransformCbuf : public Bindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX viewProj;
		};
	public:
		SkyboxTransformCbuf( Graphics& gfx, UINT slot = 0u );
		void Bind( Graphics& gfx ) noexcept(!IS_DEBUG) override;
	protected:
		void UpdateBind( Graphics& gfx, const Transforms& tf ) noexcept(!IS_DEBUG);
		Transforms GetTransforms( Graphics& gfx ) noexcept(!IS_DEBUG);
	private:
		std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
	};
}
#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"

namespace Bind
{
	class TransformCbuf : public Bindable
	{
	public:
		TransformCbuf( Graphics& gfx, UINT slot = 0u );
		void Bind( Graphics& gfx ) noexcept override;
		void InitializeParentReference( const Drawable& parent ) noexcept override;
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modelViewProj;
			DirectX::XMMATRIX modelView;
		};
		void UpdateBind( Graphics& gfx, const Transforms& tf ) noexcept;
		Transforms GetTransforms( Graphics& gfx ) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable* pParent = nullptr;
	};
}
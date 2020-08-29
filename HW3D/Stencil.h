#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	class Stencil : public Bindable
	{
	public:
		enum class Mode
		{
			Off,
			Mask,
			Write
		};
		Stencil( Graphics& gfx, Mode mode )
		{
			INFOMANAGER( gfx );
			D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

			if ( mode == Mode::Write )
			{
				dsDesc.StencilEnable = TRUE;
				dsDesc.StencilWriteMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			}
			else if ( mode == Mode::Mask )
			{
				dsDesc.DepthEnable = FALSE;
				dsDesc.StencilEnable = TRUE;
				dsDesc.StencilReadMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			}

			GFX_THROW_INFO( GetDevice( gfx )->CreateDepthStencilState( &dsDesc, &pStencil ) );
		}
		void Bind( Graphics& gfx ) noexcept override
		{
			GetContext( gfx )->OMSetDepthStencilState( pStencil.Get(), 0xFF );
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencil;
	};
}
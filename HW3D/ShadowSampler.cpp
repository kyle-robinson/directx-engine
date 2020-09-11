#include "ShadowSampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	ShadowSampler::ShadowSampler( Graphics& gfx )
	{
		INFOMANAGER( gfx );

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		GFX_THROW_INFO( GetDevice( gfx )->CreateSamplerState( &samplerDesc, &pSampler ) );
	}

	void ShadowSampler::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		INFOMANAGER_NOHR( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetSamplers( 1u, 1u, pSampler.GetAddressOf() ) );
	}
}
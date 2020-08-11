#include "Sampler.h"
#include "GraphicsThrowMacros.h"

Sampler::Sampler( Graphics& gfx )
{
	INFOMANAGER( gfx );

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GFX_THROW_INFO( GetDevice( gfx )->CreateSamplerState( &samplerDesc, &pSampler ) );
}

void Sampler::Bind( Graphics& gfx ) noexcept
{
	GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetSamplers( 0u, 1u, pSampler.GetAddressOf() ) );
}
#include "Sampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Sampler::Sampler( Graphics& gfx )
	{
		INFOMANAGER( gfx );

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		GFX_THROW_INFO( GetDevice( gfx )->CreateSamplerState( &samplerDesc, &pSampler ) );
	}

	void Sampler::Bind( Graphics& gfx ) noexcept
	{
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetSamplers( 0u, 1u, pSampler.GetAddressOf() ) );
	}

	std::shared_ptr<Sampler> Sampler::Resolve( Graphics& gfx )
	{
		return Codex::Resolve<Sampler>( gfx );
	}

	std::string Sampler::GenerateUID()
	{
		return typeid( Sampler ).name();
	}

	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID();
	}
}
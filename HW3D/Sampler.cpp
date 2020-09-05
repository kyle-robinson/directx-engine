#include "Sampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Sampler::Sampler( Graphics& gfx, bool anisoEnable, bool reflect )
		: anisoEnable( anisoEnable ), reflect( reflect )
	{
		INFOMANAGER( gfx );

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC( CD3D11_DEFAULT{} );
		samplerDesc.Filter = anisoEnable ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

		GFX_THROW_INFO( GetDevice( gfx )->CreateSamplerState( &samplerDesc, &pSampler ) );
	}

	void Sampler::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetSamplers( 0u, 1u, pSampler.GetAddressOf() ) );
	}

	std::shared_ptr<Sampler> Sampler::Resolve( Graphics& gfx, bool anisoEnable, bool reflect )
	{
		return Codex::Resolve<Sampler>( gfx, anisoEnable, reflect );
	}

	std::string Sampler::GenerateUID( bool anisoEnable, bool reflect )
	{
		using namespace std::string_literals;
		return typeid( Sampler ).name() + "#"s + ( anisoEnable ? "A"s : "a"s ) + ( reflect ? "R"s : "W"s );
	}

	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID( anisoEnable, reflect );
	}
}
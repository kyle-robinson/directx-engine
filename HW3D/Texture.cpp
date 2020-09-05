#include "Texture.h"
#include "Surface.h"
#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	Texture::Texture( Graphics& gfx, const std::string& path, UINT slot ) : slot( slot ), path( path )
	{
		INFOMANAGER( gfx );

		// load surface
		const auto s = Surface::FromFile( path );
		hasAlpha = s.AlphaLoaded();

		// create texture resource
		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		textureDesc.Width = s.GetWidth();
		textureDesc.Height = s.GetHeight();
		textureDesc.MipLevels = 0u;
		textureDesc.ArraySize = 1u;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1u;
		textureDesc.SampleDesc.Quality = 0u;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0u;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D( &textureDesc, nullptr, &pTexture ) );

		// write image data to top mip level
		GFX_THROW_INFO_ONLY( GetContext( gfx )->UpdateSubresource(
			pTexture.Get(), 0u, nullptr, s.GetBufferPtrConst(), s.GetWidth() * sizeof( Surface::Color ), 0u
		) );

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;
		GFX_THROW_INFO( GetDevice( gfx )->CreateShaderResourceView( pTexture.Get(), &srvDesc, &pTextureView ) );

		// generate mip chain
		GFX_THROW_INFO_ONLY( GetContext( gfx )->GenerateMips( pTextureView.Get() ) );
	}

	void Texture::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		INFOMANAGER_NOHR( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetShaderResources( slot, 1u, pTextureView.GetAddressOf() ) );
	}

	std::shared_ptr<Texture> Texture::Resolve( Graphics& gfx, const std::string& path, UINT slot )
	{
		return Codex::Resolve<Texture>( gfx, path, slot );
	}

	std::string Texture::GenerateUID( const std::string& path, UINT slot )
	{
		using namespace std::string_literals;
		return typeid( Texture ).name() + "#"s + path + "#" + std::to_string( slot );
	}

	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID( path, slot );
	}

	bool Texture::HasAlpha() const noexcept
	{
		return hasAlpha;
	}
}
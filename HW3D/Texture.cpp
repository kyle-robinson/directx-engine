#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"

Texture::Texture( Graphics& gfx, const Surface& s )
{
	INFOMANAGER( gfx );

	// create texture resource
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.Width = (UINT)s.GetWidth();
	textureDesc.Height = (UINT)s.GetHeight();
	textureDesc.MipLevels = 1u;
	textureDesc.ArraySize = 1u;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1u;
	textureDesc.SampleDesc.Quality = 0u;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0u;
	textureDesc.MiscFlags = 0u;

	D3D11_SUBRESOURCE_DATA sd = { 0 };
	sd.pSysMem = s.GetBufferPtr();
	sd.SysMemPitch = (UINT)s.GetWidth() * sizeof( Surface::Color );

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D( &textureDesc, &sd, &pTexture ) );

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0u;
	srvDesc.Texture2D.MipLevels = 1u;
	GFX_THROW_INFO( GetDevice( gfx )->CreateShaderResourceView( pTexture.Get(), &srvDesc, &pTextureView ) );
}

void Texture::Bind( Graphics& gfx ) noexcept
{
	GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetShaderResources( 0u, 1u, pTextureView.GetAddressOf() ) );
}
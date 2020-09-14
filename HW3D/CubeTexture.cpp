#include "CubeTexture.h"
#include "Surface.h"
#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include <vector>

namespace Bind
{
	CubeTexture::CubeTexture( Graphics& gfx, const std::string& path, UINT slot ) :
		slot( slot ), path( path )
	{
		INFOMANAGER( gfx );

		// load 6 surfaces
		std::vector<Surface> surfaces;
		for ( int i = 0; i < 6; i++ )
			surfaces.push_back( Surface::FromFile( path + "\\" + std::to_string( i ) + ".png" ) );

		// load texture data
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = surfaces[0].GetWidth();
		textureDesc.Height = surfaces[0].GetHeight();
		textureDesc.MipLevels = 1u;
		textureDesc.ArraySize = 6u;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1u;
		textureDesc.SampleDesc.Quality = 0u;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0u;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// subresource data
		D3D11_SUBRESOURCE_DATA srData[6];
		for ( int i = 0; i < 6; i++ )
		{
			srData[i].pSysMem = surfaces[i].GetBufferPtrConst();
			srData[i].SysMemPitch = surfaces[i].GetBytePitch();
			srData[i].SysMemSlicePitch = 0u;
		}

		// create texture resource
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D( &textureDesc, srData, &pTexture ) );

		// create resource view on texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0u;
		srvDesc.Texture2D.MipLevels = 1u;

		GFX_THROW_INFO( GetDevice( gfx )->CreateShaderResourceView( pTexture.Get(), &srvDesc, &pTextureView ) );
	}

	void CubeTexture::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		INFOMANAGER_NOHR( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetShaderResources( slot, 1u, pTextureView.GetAddressOf() ) );
	}

	CubeTargetTexture::CubeTargetTexture( Graphics& gfx,UINT width,UINT height,UINT slot,DXGI_FORMAT format )
		:
		slot( slot )
	{
		INFOMANAGER( gfx );

		// texture descriptor
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// create the texture resource
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D(
			&textureDesc,nullptr,&pTexture
		) );

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO( GetDevice( gfx )->CreateShaderResourceView(
			pTexture.Get(),&srvDesc,&pTextureView
		) );

		// make render target resources for capturing shadow map
		for( UINT face = 0; face < 6; face++ )
		{
			renderTargets.push_back( std::make_shared<OutputOnlyRenderTarget>( gfx,pTexture.Get(),face ) );
		}
	}

	void CubeTargetTexture::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		INFOMANAGER_NOHR( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetShaderResources( slot,1u,pTextureView.GetAddressOf() ) );
	}

	std::shared_ptr<OutputOnlyRenderTarget> Bind::CubeTargetTexture::GetRenderTarget( size_t index ) const
	{
		return renderTargets[index];
	}

	DepthCubeTexture::DepthCubeTexture( Graphics& gfx, UINT size, UINT slot ) :
		slot( slot )
	{
		INFOMANAGER( gfx );

		// initialize texture
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = size;
		textureDesc.Height = size;
		textureDesc.MipLevels = 1u;
		textureDesc.ArraySize = 6u;
		textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		textureDesc.SampleDesc.Count = 1u;
		textureDesc.SampleDesc.Quality = 0u;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		textureDesc.CPUAccessFlags = 0u;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// create texture resource
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D( &textureDesc, nullptr, &pTexture ) );

		// create view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MipLevels = 1u;
		srvDesc.Texture2D.MostDetailedMip = 0u;

		GFX_THROW_INFO( GetDevice( gfx )->CreateShaderResourceView( pTexture.Get(), &srvDesc, &pTextureView ) );

		// make depth buffer resources
		for ( UINT face = 0; face < 6; face++ )
			depthBuffers.push_back( std::make_shared<OutputOnlyDepthStencil>( gfx, pTexture, face ) );
	}

	void DepthCubeTexture::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		INFOMANAGER_NOHR( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSGetShaderResources( slot, 1u, pTextureView.GetAddressOf() ) );
	}

	std::shared_ptr<OutputOnlyDepthStencil> DepthCubeTexture::GetDepthBuffer( size_t index ) const
	{
		return depthBuffers[index];
	}
}
#include "DepthStencil.h"
#include "Graphics.h"
#include "RenderTarget.h"
#include "GraphicsThrowMacros.h"
#include <stdexcept>

namespace Bind
{
	DXGI_FORMAT MapUsageTypeless( DepthStencil::Usage usage )
	{
		switch ( usage )
		{
		case DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		case DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		}
		throw std::runtime_error( "Failed to set base usage for DepthStencil format map!" );
	}

	DXGI_FORMAT MapUsageTyped( DepthStencil::Usage usage )
	{
		switch ( usage )
		{
		case DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		case DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		}
		throw std::runtime_error( "Failed to set typed usage for DepthStencil in format map!" );
	}

	DXGI_FORMAT MapUsageColored( DepthStencil::Usage usage )
	{
		switch ( usage )
		{
		case DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		}
		throw std::runtime_error( "Failed to set colored usage for DepthStencil format map!" );
	}

	DepthStencil::DepthStencil( Graphics& gfx, UINT width, UINT height, bool canBindShaderInput, Usage usage ) :
		width( width ), height( height )
	{
		INFOMANAGER( gfx );

		// create depth stencil texture
		D3D11_TEXTURE2D_DESC depthDesc = {};
		depthDesc.Width = width;
		depthDesc.Height = height;
		depthDesc.MipLevels = 1u;
		depthDesc.ArraySize = 1u;
		depthDesc.Format = MapUsageTypeless( usage );
		depthDesc.SampleDesc.Count = 1u;
		depthDesc.SampleDesc.Quality = 0u;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | ( canBindShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0 );
		
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
		GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D( &depthDesc, nullptr, &pDepthStencil ) );

		// create target view of depth stencil texture
		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
		viewDesc.Format = MapUsageTyped( usage );
		viewDesc.Flags = 0;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		GFX_THROW_INFO( GetDevice( gfx )->CreateDepthStencilView( pDepthStencil.Get(), &viewDesc, &pDepthStencilView ) );
	}

	void DepthStencil::BindAsBuffer( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		GetContext( gfx )->OMSetRenderTargets( 0, nullptr, pDepthStencilView.Get() );
	}

	void DepthStencil::BindAsBuffer( Graphics& gfx, BufferResource* renderTarget ) noexcept(!IS_DEBUG)
	{
		assert( dynamic_cast<RenderTarget*>( renderTarget ) != nullptr );
		BindAsBuffer( gfx, static_cast<RenderTarget*>( renderTarget ) );
	}

	void DepthStencil::BindAsBuffer( Graphics& gfx, RenderTarget* rt ) noexcept(!IS_DEBUG)
	{
		rt->BindAsBuffer( gfx,this );
	}

	void DepthStencil::Clear( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		GetContext( gfx )->ClearDepthStencilView( pDepthStencilView.Get(),D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f,0u );
	}
	
	
	Surface DepthStencil::ToSurface( Graphics& gfx, bool linearize ) const
	{
		INFOMANAGER( gfx );

		// create temp texture compatible with source, with CPU read access
		Microsoft::WRL::ComPtr<ID3D11Resource> pResSource;
		pDepthStencilView->GetResource( &pResSource );

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexSource;
		pResSource.As( &pTexSource );

		D3D11_TEXTURE2D_DESC textureDesc = {};
		pTexSource->GetDesc( &textureDesc );
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.BindFlags = 0;
		
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexTemp;
		GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D( &textureDesc, nullptr, &pTexTemp ) );

		// copy contents of texture
		GFX_THROW_INFO_ONLY( GetContext( gfx )->CopyResource( pTexTemp.Get(), pTexSource.Get() ) );

		// create surface, moving temp texture data into it
		const auto width = GetWidth();
		const auto height = GetHeight();
		Surface surface{ width, height };

		D3D11_MAPPED_SUBRESOURCE msr = {};
		GFX_THROW_INFO( GetContext( gfx )->Map( pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr ) );

		auto pSrcBytes = static_cast<const char*>( msr.pData );
		for ( unsigned int y = 0; y < height; y++ )
		{
			struct Pixel
			{
				char data[4];
			};
			auto pSrcRow = reinterpret_cast<const Pixel*>( pSrcBytes + msr.RowPitch * size_t( y ) );
			for ( unsigned int x = 0; x < width; x++ )
			{
				if ( textureDesc.Format == DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS )
				{
					const auto raw = 0xFFFFFF & *reinterpret_cast<const unsigned int*>( pSrcRow + x );
					if ( linearize )
					{
						const auto normalized = (float)raw / (float)0xFFFFFF;
						const auto linearized = 0.01f / ( 1.01f - normalized );
						const auto channel = unsigned char( linearized * 255.0f );
						surface.PutPixel( x, y, { channel, channel, channel } );
					}
					else
					{
						const unsigned char channel = raw >> 16;
						surface.PutPixel( x, y, { channel, channel, channel } );
					}
				}
				else if ( textureDesc.Format == DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS )
				{
					const auto raw = *reinterpret_cast<const unsigned int*>( pSrcRow + x );
					if ( linearize )
					{
						const auto linearized = 0.01f / ( 1.01f - raw );
						const auto channel = unsigned char( linearized * 255.0f );
						surface.PutPixel( x, y, { channel, channel, channel } );
					}
					else
					{
						const auto channel = unsigned char( raw * 255.0f );
						surface.PutPixel( x, y, { channel, channel, channel } );
					}
				}
				else
				{
					throw std::runtime_error( "Unable to convert from DepthStencil to Surface : Bad Format" );
				}
			}
		}
		GFX_THROW_INFO_ONLY( GetContext( gfx )->Unmap( pTexTemp.Get(), 0 ) );
		return surface;
	}

	unsigned int DepthStencil::GetWidth() const
	{
		return width;
	}

	unsigned int DepthStencil::GetHeight() const
	{
		return height;
	}

	ShaderInputDepthStencil::ShaderInputDepthStencil( Graphics& gfx, UINT slot, Usage usage )
		: ShaderInputDepthStencil( gfx, gfx.GetWidth(), gfx.GetHeight(), slot, usage )
	{ }

	ShaderInputDepthStencil::ShaderInputDepthStencil( Graphics& gfx, UINT width, UINT height, UINT slot, Usage usage )
		: DepthStencil( gfx, width, height, true, usage ), slot( slot )
	{
		INFOMANAGER( gfx );

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = MapUsageColored( usage ); // this will need to be fixed
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		Microsoft::WRL::ComPtr<ID3D11Resource> pRes;
		pDepthStencilView->GetResource( &pRes );
		GFX_THROW_INFO( GetDevice( gfx )->CreateShaderResourceView( pRes.Get(),&srvDesc,&pShaderResourceView ) );
	}

	void ShaderInputDepthStencil::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		GetContext( gfx )->PSSetShaderResources( slot,1u,pShaderResourceView.GetAddressOf() );
	}


	OutputOnlyDepthStencil::OutputOnlyDepthStencil( Graphics& gfx )
		:
		OutputOnlyDepthStencil( gfx,gfx.GetWidth(),gfx.GetHeight() )
	{}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil( Graphics& gfx, UINT width, UINT height )
		:
		DepthStencil( gfx, width, height, false, Usage::DepthStencil )
	{}

	void OutputOnlyDepthStencil::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		assert( "OutputOnlyDepthStencil cannot be bound as shader input!" && false );
	}
}
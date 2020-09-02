#include "DepthStencil.h"
#include "GraphicsThrowMacros.h"

DepthStencil::DepthStencil( Graphics& gfx, UINT width, UINT height )
{
	INFOMANAGER( gfx );

	// create and bind depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = { 0 };
	descDepth.Width = (UINT)width;
	descDepth.Height = (UINT)height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
	GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D( &descDepth, nullptr, &pDepthStencil ) );

	// create target view of depth stencil texture
	GFX_THROW_INFO( GetDevice( gfx )->CreateDepthStencilView( pDepthStencil.Get(), nullptr, &pDepthStencilView ) );
}

void DepthStencil::BindAsDepthStencil( Graphics& gfx ) const noexcept
{
	GetContext( gfx )->OMSetRenderTargets( 0, nullptr, pDepthStencilView.Get() );
}

void DepthStencil::Clear( Graphics& gfx ) const noexcept
{
	GetContext( gfx )->ClearDepthStencilView( pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u );
}
#include "Graphics.h"
#include "dxerr.h"
#include <sstream>

#pragma comment(lib, "d3d11.lib")

#define GFX_THROW_FAILED( hrcall ) if( FAILED( hr == hrcall ) ) throw Graphics::HrException( __LINE__, __FILE__, hr )
#define GFX_DEVICE_REMOVED_EXCEPT( hr ) Graphics::DeviceRemovedException( __LINE__, __FILE__, ( hr ) );

Graphics::Graphics( HWND hWnd )
{
	DXGI_SWAP_CHAIN_DESC sd = {0};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	HRESULT hr;

	// create device, front/back buffers, swap chain and rendering context
	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));

	// gain access to back buffer (sub-resource)
	ID3D11Resource* pBackBuffer = nullptr;
	GFX_THROW_FAILED( pSwap->GetBuffer( 0, __uuidof( ID3D11Resource ), reinterpret_cast<void**>( &pBackBuffer ) ) );
	GFX_THROW_FAILED( pDevice->CreateRenderTargetView( pBackBuffer, nullptr, &pTarget ) ) ;
	pBackBuffer->Release();
}
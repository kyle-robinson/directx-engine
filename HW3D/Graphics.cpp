#include "Graphics.h"
#include "dxerr.h"
#include <sstream>

#pragma comment(lib, "d3d11.lib")

#define GFX_EXCEPT_NOINFO( hr ) Graphics::HrException( __LINE__, __FILE__, ( hr ) )
#define GFX_THROW_NOINFO( hrcall ) if( FAILED( hr = hrcall ) ) throw Graphics::HrException( __LINE__, __FILE__, hr )
//#define GFX_THROW_FAILED( hrcall ) if( FAILED( hr == hrcall ) ) throw Graphics::HrException( __LINE__, __FILE__, hr )

#ifndef NDEBUG
#define GFX_EXCEPT( hr ) Graphics::HrException( __LINE__, FILE__, ( hr ), infoManager.GetMessages )
#define GFX_THROW_INFO( hrcall ) infoManager.Set(); if( FAILED( hr = ( hrcall ) ) ) throw GFX_EXCEPT( hr )
#define GFX_DEVICE_REMOVED_EXCEPT( hr ) Graphics::DeviceRemovedException( __LINE__, __FILE__, ( hr ), infoManager.GetMessages() )

#define GFX_EXCEPT( hr ) Graphics::HrException( __LINE__, __FILE__, ( hr ) )
#define GFX_THROW_INFO( hrcall ) GFX_THROW_NOINFO( hrcall )
#define GFX_DEVICE_REMOVED_EXCEPT( hr ) Graphics::DeviceRemovedException( __LINE__, __FILE__, ( hr ) );
#endif

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

	UINT swapCreateFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr;

	// create device, front/back buffers, swap chain and rendering context
	GFX_THROW_INFO( D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	) );

	// gain access to back buffer (sub-resource)
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO( pSwap->GetBuffer( 0, __uuidof( ID3D11Resource ), &pBackBuffer ) );
	GFX_THROW_INFO( pDevice->CreateRenderTargetView( pBackBuffer.Get(), nullptr, pTarget.GetAddressOf() ) ) ;
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if ( FAILED( hr = pSwap->Present( 1u, 0u ) ) )
	{
		if ( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw GFX_DEVICE_REMOVED_EXCEPT( pDevice->GetDeviceRemovedReason() );
		}
		else
		{
			throw GFX_EXCEPT( hr );
		}
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue };
	pContext->ClearRenderTargetView( pTarget.Get(), color );
}

Graphics::HrException::HrException( int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs ) noexcept : GfxException(line, file), hr(hr)
{
	// join all info messages with newlines into string
	for ( const auto& m : infoMsgs )
	{
		info += m;
		info.push_back( '\n' );
	}
	// remove final newline if present
	if ( !info.empty() )
		info.pop_back();
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if ( !info.empty() )
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{	
	return DXGetErrorStringA( hr );
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	WCHAR buf[512];
	DXGetErrorDescription( hr, buf, sizeof( buf ) );

	std::wstring ws(buf);
	std::string str(ws.begin(), ws.end());

	return str;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
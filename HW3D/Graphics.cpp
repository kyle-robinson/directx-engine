#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define GFX_EXCEPT_NOINFO( hr ) Graphics::HrException( __LINE__, __FILE__, ( hr ) )
#define GFX_THROW_NOINFO( hrcall ) if( FAILED( hr = hrcall ) ) throw Graphics::HrException( __LINE__, __FILE__, hr )
//#define GFX_THROW_FAILED( hrcall ) if( FAILED( hr == hrcall ) ) throw Graphics::HrException( __LINE__, __FILE__, hr )

#ifndef NDEBUG
#define GFX_EXCEPT( hr ) Graphics::HrException( __LINE__, FILE__, ( hr ), infoManager.GetMessages )
#define GFX_THROW_INFO( hrcall ) infoManager.Set(); if( FAILED( hr = ( hrcall ) ) ) throw GFX_EXCEPT( hr )
#define GFX_DEVICE_REMOVED_EXCEPT( hr ) Graphics::DeviceRemovedException( __LINE__, __FILE__, ( hr ), infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY( call ) infoManager.Set(); { auto v = infoManager.GetMessages(); if ( !v.empty() ) { throw Graphics::InfoException( __LINE__, __FILE__, v ) } }

#define GFX_EXCEPT( hr ) Graphics::HrException( __LINE__, __FILE__, ( hr ) )
#define GFX_THROW_INFO( hrcall ) GFX_THROW_NOINFO( hrcall )
#define GFX_DEVICE_REMOVED_EXCEPT( hr ) Graphics::DeviceRemovedException( __LINE__, __FILE__, ( hr ) );
#define GFX_THROW_INFO_ONLY( call ) ( call )
#endif

Graphics::Graphics( HWND hWnd, int width, int height ) : width( width ), height( height )
{
	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
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

void Graphics::DrawTriangle()
{
	HRESULT hr;
	
	struct Vertex
	{
		struct
		{
			float x, y;
		} pos;
		struct
		{
			unsigned char r, g, b, a;
		} color;
	};

	// create vertex buffer
	const Vertex vertices[] =
	{
		// positions		color
		{ 0.0f,   0.5f,		255, 0,   0,   0 },
		{ 0.5f,  -0.5f,		0,   255, 0,   0 },
		{ -0.5f, -0.5f,		0,   0,   255, 0 },
		{ -0.3f, 0.3f,		0,   255, 0,   0 },
		{ 0.3f,  0.3f,		0,   0,   255, 0 },
		{ 0.0f,  -0.8f,		255, 0,   0,   0 }
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = { 0 };
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof( vertices );
	bd.StructureByteStride = sizeof( Vertex );

	D3D11_SUBRESOURCE_DATA sd = { 0 };
	sd.pSysMem = vertices;

	GFX_THROW_INFO( pDevice->CreateBuffer( &bd, &sd, &pVertexBuffer ) );

	// bind vertex buffer to pipeline
	const UINT stride = sizeof( Vertex );
	const UINT offset = 0u;
	pContext->IASetVertexBuffers( 0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset );

	// create index buffer
	const unsigned short indices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 4, 1,
		2, 1, 5
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof( indices );
	ibd.StructureByteStride = sizeof( unsigned short );

	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = indices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	// bind index buffer
	pContext->IASetIndexBuffer( pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u );

	// create pixel shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO( D3DReadFileToBlob( L"PixelShader.cso", &pBlob ) );
	GFX_THROW_INFO( pDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader ) );

	// bind pixel shader
	pContext->PSSetShader( pPixelShader.Get(), nullptr, 0u );

	// create vertex shader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO( D3DReadFileToBlob( L"VertexShader.cso", &pBlob ) );
	GFX_THROW_INFO( pDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader ) );

	// bind vertex shader
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// input (vertex) layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	GFX_THROW_INFO( pDevice->CreateInputLayout( ied, (UINT)std::size( ied ), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout ) );

	// bind vertex layout
	pContext->IASetInputLayout( pInputLayout.Get() );

	// bind render target
	pContext->OMSetRenderTargets( 1u, pTarget.GetAddressOf(), nullptr );

	// set primitive topology to triangle list
	pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = GetWidth();
	vp.Height = GetHeight();
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports( 1u, &vp );

	GFX_THROW_INFO_ONLY( pContext->DrawIndexed( (UINT)std::size( indices ), 0u, 0u ) );
}

UINT Graphics::GetWidth() const noexcept
{
	return width;
}

UINT Graphics::GetHeight() const noexcept
{
	return height;
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

Graphics::InfoException::InfoException( int line, const char* file, std::vector<std::string> infoMsgs ) noexcept : GfxException( line, file )
{
	// join all info messages
	for ( const auto& m : infoMsgs )
	{
		info += m;
		info.push_back( '\n' );
	}
	// remove final newline
	if ( !info.empty() )
		info.pop_back();
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
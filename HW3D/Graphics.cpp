#include "Graphics.h"
#include "GraphicsThrowMacros.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

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
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pTarget.GetAddressOf()));

	// create and bind depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// create and bind depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = { 0 };
	descDepth.Width = (UINT)GetWidth();
	descDepth.Height = (UINT)GetHeight();
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	// create view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = { };
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;

	GFX_THROW_INFO( pDevice->CreateDepthStencilView( pDepthStencil.Get(), &descDSV, &pDSV ) );
	pContext->OMSetRenderTargets( 1u, pTarget.GetAddressOf(), pDSV.Get() );

	// configure viewports
	D3D11_VIEWPORT vp;
	vp.Width = (UINT)GetWidth();
	vp.Height = (UINT)GetHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports( 1u, &vp );

	ImGui_ImplDX11_Init( pDevice.Get(), pContext.Get() );
}

void Graphics::BeginFrame( float red, float green, float blue ) noexcept
{
	if ( imguiEnabled )
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[] = { red, green, blue };
	pContext->ClearRenderTargetView( pTarget.Get(), color );
	pContext->ClearDepthStencilView( pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u );
}

void Graphics::EndFrame()
{
	if ( imguiEnabled )
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
	}
	
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

void Graphics::DrawTriangle( float angle, float x, float y )
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
	sd.pSysMem = &vertices;

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
	isd.pSysMem = &indices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	// bind index buffer
	pContext->IASetIndexBuffer( pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u );

	// create constant buffer for matrix transformation
	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};

	const ConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationZ( angle ) *
				DirectX::XMMatrixScaling( 3.0f / 4.0f, 1.0f, 1.0f ) *
				DirectX::XMMatrixTranslation( x, y, 0.0f )
			)
		}
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof( cb );
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = { 0 };
	csd.pSysMem = &cb;
	GFX_THROW_INFO( pDevice->CreateBuffer( &cbd, &csd, &pConstantBuffer ) );

	// bind constant buffer to vertex shader
	pContext->VSSetConstantBuffers( 0u, 1u, pConstantBuffer.GetAddressOf() );

	// create pixel shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO( D3DReadFileToBlob( L"PixelShaderTri.cso", &pBlob ) );
	GFX_THROW_INFO( pDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader ) );

	// bind pixel shader
	pContext->PSSetShader( pPixelShader.Get(), nullptr, 0u );

	// create vertex shader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO( D3DReadFileToBlob( L"VertexShaderTri.cso", &pBlob ) );
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

void Graphics::DrawCube( float angle, float x, float z )
{
	HRESULT hr;

	struct Vertex
	{
		struct
		{
			float x, y, z;
		} pos;
	};

	const Vertex vertices[] =
	{
		// positions
		{ -1.0f, -1.0f, -1.0f },
		{  1.0f, -1.0f, -1.0f },
		{ -1.0f,  1.0f, -1.0f },
		{  1.0f,  1.0f, -1.0f },
		{ -1.0f, -1.0f,  1.0f },
		{  1.0f, -1.0f,  1.0f },
		{ -1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f }
	};

	// create and bind vertex buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof( vertices );
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.StructureByteStride = sizeof( Vertex );
	
	D3D11_SUBRESOURCE_DATA bsd = { 0 };
	bsd.pSysMem = &vertices;
	GFX_THROW_INFO( pDevice->CreateBuffer( &bd, &bsd, &pVertexBuffer ) );

	const UINT stride = sizeof( Vertex );
	const UINT offset = 0u;
	pContext->IASetVertexBuffers( 0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset );

	// create and bind index buffer
	const unsigned short indices[] =
	{
		0, 2, 1,	2, 3, 1,
		1, 3, 5,	3, 7, 5,
		2, 6, 3,	3, 6, 7,
		4, 5, 7,	4, 7, 6,
		0, 4, 2,	2, 4, 6,
		0, 1, 4,	1, 5, 4
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.ByteWidth = sizeof( indices );
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.StructureByteStride = sizeof( unsigned short );
	
	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = &indices;

	GFX_THROW_INFO( pDevice->CreateBuffer( &ibd, &isd, &pIndexBuffer ) );
	pContext->IASetIndexBuffer( pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u );

	// create and bind constant buffer
	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};

	const ConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationZ( angle ) *
				DirectX::XMMatrixRotationX( angle ) *
				DirectX::XMMatrixTranslation( x, 0.0f, z + 4.0f ) *
				DirectX::XMMatrixPerspectiveLH( 1.0f, 3.0f / 4.0f, 0.5f, 10.0f )
			)
		}
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.ByteWidth = sizeof( cb );
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd = { 0 };
	csd.pSysMem = &cb;

	GFX_THROW_INFO( pDevice->CreateBuffer( &cbd, &csd, &pConstantBuffer ) );
	pContext->VSSetConstantBuffers( 0u, 1u, pConstantBuffer.GetAddressOf() );

	// create and bind constant color buffer
	struct ConstantBuffer2
	{
		struct
		{
			float r, g, b, a;
		} face_colors[6];
	};

	const ConstantBuffer2 cb2 =
	{
		{
			{ 1.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 0.0f },
			{ 0.0f, 1.0f, 1.0f }
		}
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer2;
	D3D11_BUFFER_DESC cbd2 = { 0 };
	cbd2.ByteWidth = sizeof( cb2 );
	cbd2.Usage = D3D11_USAGE_DEFAULT;
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.CPUAccessFlags = 0u;
	cbd2.MiscFlags = 0u;
	cbd2.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd2 = { 0 };
	csd2.pSysMem = &cb2;
	
	GFX_THROW_INFO( pDevice->CreateBuffer( &cbd2, &csd2, &pConstantBuffer2 ) );
	pContext->PSSetConstantBuffers( 0u, 1u, pConstantBuffer2.GetAddressOf() );

	// create and bind pixel shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO( D3DReadFileToBlob( L"PixelShaderCube.cso", &pBlob ) );
	GFX_THROW_INFO( pDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader ) );
	pContext->PSSetShader( pPixelShader.Get(), nullptr, 0u );

	// create and bind vertex shader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO( D3DReadFileToBlob( L"VertexShaderCube.cso", &pBlob ) );
	GFX_THROW_INFO( pDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader ) );
	pContext->VSSetShader( pVertexShader.Get(), nullptr, 0u );

	// input layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	GFX_THROW_INFO( pDevice->CreateInputLayout( ied, (UINT)std::size( ied ), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout ) );
	pContext->IASetInputLayout( pInputLayout.Get() );

	// set topology
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

	// draw cube
	GFX_THROW_INFO_ONLY( pContext->DrawIndexed( (UINT)std::size( indices ), 0u, 0u ) );
}

void Graphics::DrawIndexed( UINT count ) noexcept(!IS_DEBUG)
{
	GFX_THROW_INFO_ONLY( pContext->DrawIndexed( count, 0u, 0u ) );
}

void Graphics::SetProjection( DirectX::FXMMATRIX proj ) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

void Graphics::SetCamera( DirectX::FXMMATRIX cam ) noexcept
{
	camera = cam;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}

void Graphics::EnableImGui() noexcept
{
	imguiEnabled = true;
}

void Graphics::DisableImGui() noexcept
{
	imguiEnabled = false;
}

bool Graphics::IsImGuiEnabled() const noexcept
{
	return imguiEnabled;
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

	std::wstring ws( buf );
	std::string str( ws.begin(), ws.end() );

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
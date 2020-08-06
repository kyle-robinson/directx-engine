#pragma once
#include "WindowsInclude.h"
#include "Exception.h"
#include <d3d11.h>

class Graphics
{
public:
	class GfxException : public Exception
	{
		using Exception::Exception;
	};
	class HrException : public GfxException
	{
	public:
		HrException( int line, const char* file, HRESULT hr ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	};
public:
	Graphics( HWND hWnd );
	Graphics( const Graphics&  ) = delete;
	Graphics& operator = ( const Graphics& ) = delete;
	~Graphics();
	void EndFrame();
	void ClearBuffer( float red, float green, float blue ) noexcept
	{
		const float color[] = { red, green, blue };
		pContext->ClearRenderTargetView( pTarget, color );
	}
private:
	IDXGISwapChain* pSwap = nullptr;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};
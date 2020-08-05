#pragma once
#include "WindowsInclude.h"
#include <d3d11.h>

class Graphics
{
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
#pragma once
#include "WindowsInclude.h"
#include "Exception.h"
#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>

#include "DxgiInfoManager.h"
#include <vector>
#include <string>

class Graphics
{
	friend class Bindable;
public:
	class GfxException : public Exception
	{
		using Exception::Exception;
	};
	class HrException : public GfxException
	{
	public:
		HrException( int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {} ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public GfxException
	{
	public:
		InfoException( int line, const char* file, std::vector<std::string> infoMsgs ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
		std::string reason;
	};
public:
	Graphics( HWND hWnd, int width, int height );
	Graphics( const Graphics&  ) = delete;
	Graphics& operator = ( const Graphics& ) = delete;
	~Graphics() = default;
	void BeginFrame( float red, float green, float blue ) noexcept;
	void EndFrame();
	void DrawTriangle( float angle, float x, float y );
	void DrawCube( float angle, float x, float z );
	void DrawIndexed( UINT count ) noexcept(!IS_DEBUG);
	void SetProjection( DirectX::FXMMATRIX proj ) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera( DirectX::FXMMATRIX cam ) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;
	void EnableImGui() noexcept;
	void DisableImGui() noexcept;
	bool IsImGuiEnabled() const noexcept;
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
private:
	bool imguiEnabled = true;
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	UINT width;
	UINT height;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
};
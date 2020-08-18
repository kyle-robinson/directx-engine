#pragma once
#include "WindowsInclude.h"
#include "Exception.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "WindowsThrowMacros.h"
#include <string>
#include <optional>
#include <memory>

class Window
{
public:
	class WindowException : public Exception
	{
		using Exception::Exception;
	public:
		static std::string TranslateErrorCode( HRESULT hr ) noexcept;
	};
	class HrException : public WindowException
	{
	public:
		HrException( int line, const char* file, HRESULT hr ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;

		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public WindowException
	{
		using WindowException::WindowException;
	public:
		const char* GetType() const noexcept override;
	};
	/*class WindowException : public Exception
	{
	public:
		WindowException( int line, const char* file, HRESULT hr ) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		static std::string TranslateErrorCode( HRESULT hr );
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};*/
private:
	class WindowClass
	{
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass( const WindowClass& ) = delete;
		WindowClass& operator = ( const WindowClass& ) = delete;
		static constexpr const wchar_t* wndClassName = L"DirectX 11 Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window( int width, int height, const char* name );
	~Window();
	Window( const Window& ) = delete;
	Window& operator = ( const Window& ) = delete;
	void SetTitle( const std::wstring& title );
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	static std::optional<int> ProcessMessages() noexcept;
	Graphics& Gfx();
private:
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void ShowCursor() noexcept;
	void HideCursor() noexcept;
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
	static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT WINAPI HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd;
	Mouse mouse;
private:
	bool cursorEnabled = false;
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
};
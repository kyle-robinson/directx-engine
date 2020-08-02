#pragma once
#include "WindowsInclude.h"
#include "Exception.h"
#include <string>
#include <optional>

class Window
{
public:
	class WindowException : public Exception
	{
		WindowException( int line, const char* file, HRESULT hr ) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		static std::string TranslateErrorCode( HRESULT hr );
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
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
	Window( int widht, int height, const wchar_t* name ) noexcept;
	~Window();
	Window( const Window& ) = delete;
	Window& operator = ( const Window& ) = delete;
	void SetTitle( const std::wstring& title );
private:
	static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT WINAPI HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width;
	int height;
	HWND hWnd;
};

#define WND_EXCEPT( hr ) Window::WindowException( __LINE__, __FILE__, hr )
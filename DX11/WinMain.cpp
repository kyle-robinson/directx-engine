#include "ErrorLogger.h"
#include <string>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DirectXTK.lib" )

class Window
{
public:
	Window( int width, int height, const std::wstring& name );
	~Window() noexcept;
	void ProcessMessages() noexcept;
	HINSTANCE GetInstance() noexcept;
	const std::wstring& GetName() noexcept;
	int GetWidth() const noexcept;
	int GetHeight() const noexcept;
private:
	HWND hWindow;
	HINSTANCE hInstance;
	int width, height;
	const std::wstring& windowName;
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
};

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	Window wnd( 1000, 800, L"DirectX 11 Engine Window" );
	wnd.ProcessMessages();
	return 0;
}

Window::Window( int width, int height, const std::wstring& name ) : width( width ), height( height ), windowName( name )
{
	// register window
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0u;
	wc.cbWndExtra = 0u;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"DirectX 11 Window Class";
	wc.hIconSm = nullptr;
	RegisterClassEx( &wc );

	// create window
	hWindow = CreateWindow(
		L"DirectX 11 Window Class", GetName().c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, GetWidth(), GetHeight(),
		nullptr, nullptr, GetInstance(), nullptr
	);

	if ( hWindow == nullptr )
		throw MessageBoxA( hWindow, "Window Creation Exception", "Failed to create window!", MB_ICONERROR | MB_OK );

	ShowWindow( hWindow, SW_SHOWDEFAULT );
}

Window::~Window()
{
	DestroyWindow( hWindow );
}

void Window::ProcessMessages() noexcept
{
	MSG msg = { 0 };
	while ( GetMessage( &msg, nullptr, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

HINSTANCE Window::GetInstance() noexcept
{
	return hInstance;
}

const std::wstring& Window::GetName() noexcept
{
	return windowName;
}

int Window::GetWidth() const noexcept
{
	return width;
}

int Window::GetHeight() const noexcept
{
	return height;
}

LRESULT CALLBACK Window::WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) noexcept
{
	switch (uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint( hWnd, &ps );
			FillRect( hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW+1) );
			EndPaint( hWnd, &ps );
		}
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
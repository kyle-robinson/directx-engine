#include "RenderWindow.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch ( uMsg )
	{
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint( hWnd, &ps );
			FillRect( hdc, &ps.rcPaint, (HBRUSH)( COLOR_WINDOW + 2 ) );
			EndPaint( hWnd, &ps );
		}
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

bool RenderWindow::Initialize( HINSTANCE hInstance, const std::string& windowName, const std::string& windowClass, int width, int height )
{
	// register window class
	this->hInstance = hInstance;
	this->width = width;
	this->height = height;
	this->windowTitle = windowName;
	this->windowTitle_Wide = StringConverter::StringToWide( windowName );
	this->windowClass = windowClass;
	this->windowClass_Wide = StringConverter::StringToWide( windowClass );

	this->RegisterWindowClass();

	// create window
	this->hWnd = CreateWindow(
		this->windowClass_Wide.c_str(),
		this->windowTitle_Wide.c_str(),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		this->width,
		this->height,
		NULL,
		NULL,
		this->hInstance,
		nullptr
	);

	if ( this->hWnd == NULL )
	{
		ErrorLogger::Log( GetLastError(), "ERROR::CreateWindow Failed for window: " + this->windowTitle );
		return false;
	}

	// show window
	ShowWindow( this->hWnd, SW_SHOW );
	SetForegroundWindow( this->hWnd );
	SetFocus( this->hWnd );

	return true;
}

bool RenderWindow::ProcessMessages() noexcept
{
	MSG msg = {};
	ZeroMemory( &msg, sizeof( MSG ) );
	while ( PeekMessage( &msg, this->hWnd, 0u, 0u, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	// check if the window was closed
	if ( msg.message == WM_NULL )
	{
		if ( !IsWindow( this->hWnd ) )
		{
			this->hWnd = NULL;
			UnregisterClass( this->windowClass_Wide.c_str(), this->hInstance );
			return false;
		}
	}

	return true;
}

void RenderWindow::RegisterWindowClass() noexcept
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof( WNDCLASSEX );
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = windowClass_Wide.c_str();
	wc.hIconSm = NULL;
	RegisterClassEx( &wc );
}

RenderWindow::~RenderWindow() noexcept
{
	if ( this->hWnd != NULL )
	{
		UnregisterClass( this->windowClass_Wide.c_str(), this->hInstance );
		DestroyWindow( this->hWnd );
	}
}
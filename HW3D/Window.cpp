#include "Window.h"
#include "resource.h"
#include <sstream>

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInst( GetModuleHandle( nullptr ) )
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof( wc );
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage( hInst, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 32, 32, 0 ));
	wc.hCursor = LoadCursor( Window::WindowClass::GetInstance(), (LPCWSTR)IDR_ANICURSOR1 );
	wc.hbrBackground = nullptr; // CreateSolidBrush(RGB(255, 0, 0))
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage( hInst, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 16, 16, 0 ));
	RegisterClassEx( &wc );
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass( wndClassName, GetInstance() );
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::Window( int width, int height, const char* name ) : width( width ), height( height )
{
	// calculate window size
	RECT wr = { 0 };
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.bottom;
	if ( AdjustWindowRect( &wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE ) == 0 )
		throw WND_LAST_EXCEPT();

	// convert 'const char*' to 'const wchar_t*'
	std::wstring w;
	std::copy(name, name + strlen(name), back_inserter(w));
	const WCHAR* wChar = w.c_str();

	// create window and get handle
	hWnd = CreateWindow(
		WindowClass::GetName(), wChar,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	if ( hWnd == nullptr )
		throw WND_LAST_EXCEPT();

	ShowWindow( hWnd, SW_SHOWDEFAULT );

	// create graphics object
	pGfx = std::make_unique<Graphics>( hWnd );
}

Window::~Window()
{
	DestroyWindow( hWnd );
}

void Window::SetTitle( const std::wstring& title )
{
	if ( SetWindowText( hWnd, title.c_str() ) == 0 )
		throw WND_LAST_EXCEPT();
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	
	// while the queue has messages - remove and dispatch them
	while ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
	{
		if ( msg.message == WM_QUIT )
		{
			// return optional wrapping int
			return ( int )msg.wParam;
		}

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	// return empty optional when not quitting app
	return {};
}

Graphics& Window::Gfx()
{
	return *pGfx;
}

LRESULT WINAPI Window::HandleMsgSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	if ( msg == WM_NCCREATE )
	{
		// extract ptr to window class
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd) );
		// set message proc to normal
		SetWindowLongPtr( hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk) );
		// forward message to the window class handler
		return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
	}
	// if message is received before the WM_NCCREATE message, handle with default handler
	return DefWindowProc( hWnd, msg, wParam, lParam );

}

LRESULT WINAPI Window::HandleMsgThunk( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	// retrieve ptr to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr( hWnd, GWLP_USERDATA ));
	// forward message to window instance handler
	return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
}

LRESULT Window::HandleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	switch ( msg )
	{
	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;

	case WM_KILLFOCUS:
		kbd.ClearState();
		break;

	/* Keyboard Messages */
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		if ( !( lParam & 0x40000000 ) || kbd.AutorepeatIsEnabled() )
			kbd.OnKeyPressed( static_cast<unsigned char>(wParam) );
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		kbd.OnKeyReleased( static_cast<unsigned char>(wParam) );
		break;

	case WM_CHAR:
		kbd.OnChar( static_cast<unsigned char>(wParam) );
		break;
	/* End Keyboard Messages */

	/* Mouse Messages */
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		// in client region
		if ( pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height )
		{
			mouse.OnMouseMove( pt.x, pt.y );
			if ( !mouse.IsInWindow() )
			{
				SetCapture( hWnd );
				mouse.OnMouseEnter();
			}
		}
		// not in client
		else
		{
			if ( wParam & ( MK_LBUTTON | MK_RBUTTON ) )
			{
				mouse.OnMouseMove( pt.x, pt.y );
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}

	case WM_LBUTTONDOWN:
	{
		SetCursor(LoadCursor( Window::WindowClass::GetInstance(), (LPCWSTR)IDR_ANICURSOR2 ));
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnLeftPressed( pt.x, pt.y );
		break;
	}

	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnRightPressed( pt.x, pt.y );
		break;
	}

	case WM_LBUTTONUP:
	{
		SetCursor(LoadCursor(Window::WindowClass::GetInstance(), (LPCWSTR)IDR_ANICURSOR1));
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnLeftReleased( pt.x, pt.y );
		break;
	}

	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnRightReleased( pt.x, pt.y );
		break;
	}

	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		const int delta = GET_WHEEL_DELTA_WPARAM( wParam );
		mouse.OnWheelDelta( pt.x, pt.y, delta );
		/*if ( GET_WHEEL_DELTA_WPARAM( wParam ) > 0 )
		{
			mouse.OnWheelUp( pt.x, pt.y );
		}
		else if ( GET_WHEEL_DELTA_WPARAM( wParam ) < 0 )
		{
			mouse.OnWheelDown( pt.x, pt.y );
		}*/
		break;
	}
	/* End of Mouse Messages */
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

Window::WindowException::WindowException( int line, const char* file, HRESULT hr ) noexcept : Exception( line, file ), hr( hr )
{
}

const char* Window::WindowException::what() const noexcept
{	
	std::stringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::WindowException::GetType() const noexcept
{
	return "Window Exception";
}

std::string Window::WindowException::TranslateErrorCode( HRESULT hr )
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr
	);
	
	if ( nMsgLen == 0 )
		return "Unidentified Error Code";

	std::string errorString = pMsgBuf;
	LocalFree( pMsgBuf );
	return errorString;
}

HRESULT Window::WindowException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::WindowException::GetErrorString() const noexcept
{
	return TranslateErrorCode( hr );
}
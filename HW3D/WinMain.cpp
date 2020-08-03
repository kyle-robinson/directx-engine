#include "Window.h"
#include <sstream>

/*LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage( 10 );
		break;

	case WM_KEYDOWN:
		if ( wParam == 'F' )
			SetWindowText( hWnd, L"It is currently raining!" );
		if ( wParam == VK_ESCAPE )
			PostQuitMessage( 10 );
		break;

	case WM_KEYUP:
		if ( wParam == 'F' )
			SetWindowText( hWnd, L"ObamaCare" );
		break;

	case WM_CHAR:
		{
			static std::wstring title;
			title.push_back( wParam );
			LPCWSTR sw = title.c_str();
			SetWindowText( hWnd, sw );
		}
		break;

	case WM_LBUTTONDOWN:
		{
			POINTS pt = MAKEPOINTS( lParam );
			std::wostringstream oss;
			oss << "(" << pt.x << "," << pt.y << ")";
			SetWindowText( hWnd, oss.str().c_str() );
		}
		break;
	}
	
	return DefWindowProc( hWnd, msg, wParam, lParam );
}*/

int CALLBACK WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			mCmdShow )
{
	/*const auto pClassName = L"HWND";
	
	// register window class
	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;
	wc.hIconSm = nullptr;
	RegisterClassEx( &wc );

	// create window instance
	HWND hWnd = CreateWindowEx(
		0, pClassName,
		L"DirectX Window",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		100, 100, 640, 480,
		nullptr, nullptr, hInstance, nullptr
	);

	ShowWindow(hWnd, SW_SHOW);*/

	try
	{
		Window wnd( 640, 480, "DirectX 11 Engine Window" );

		// handle messages
		MSG msg;
		BOOL gResult;
		while ( (gResult = GetMessage(&msg, nullptr, 0, 0)) > 0 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			
			// Keyboard input
			if ( wnd.kbd.KeyIsPressed( VK_ESCAPE ) )
			{
				PostQuitMessage( 0 );
				return 0;
			}
			if ( wnd.kbd.KeyIsPressed( VK_MENU ) )
			{
				MessageBoxA( nullptr, "You pressed SPACE!", "A key was pressed.", MB_OK | MB_ICONEXCLAMATION );
			}

			// Mouse input
			while ( !wnd.mouse.IsEmpty() )
			{
				const auto e = wnd.mouse.Read();
				switch ( e.GetType() )
				{
				case Mouse::Event::Type::Move:
					{
						std::wostringstream oss;
						oss << "Mouse Position: (" << e.GetPosX() << "," << e.GetPosY() << ")";
						wnd.SetTitle(oss.str());
					}
					break;
					
				case Mouse::Event::Type::Leave:
					wnd.SetTitle( L"Mouse Left Window!" );
					break;
				}
			}
		}

		if ( gResult == -1 )
			return -1;

		return msg.wParam;
	}
	catch ( const Exception& e )
	{
		MessageBoxA( nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION );
	}
	catch ( const std::exception& e )
	{
		MessageBoxA( nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION );
	}
	catch ( ... )
	{
		MessageBoxA( nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION );
	}
	return -1;
}
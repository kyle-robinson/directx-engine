#include "WindowContainer.h"

LRESULT CALLBACK WindowContainer::WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) noexcept
{
	switch ( uMsg )
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 2));
		EndPaint(hWnd, &ps);
		return 0;
	}

	default:
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
}
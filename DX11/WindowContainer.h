#pragma once
#include "RenderWindow.h"

class WindowContainer
{
public:
	LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) noexcept;
protected:
	RenderWindow renderWindow;
private:

};
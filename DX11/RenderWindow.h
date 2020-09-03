#pragma once
#include "ErrorLogger.h"

class RenderWindow
{
public:
	bool Initialize( HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int width, int height );
	bool ProcessMessages() noexcept;
	~RenderWindow() noexcept;
private:
	void RegisterWindowClass() noexcept;
	LRESULT WINAPI WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) noexcept;
	HWND hWnd = NULL;
	HINSTANCE hInstance = NULL;
	std::string windowTitle = "";
	std::wstring windowTitle_Wide = L"";
	std::string windowClass = "";
	std::wstring windowClass_Wide = L"";
	int width, height;
};
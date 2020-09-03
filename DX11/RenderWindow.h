#pragma once
#include "ErrorLogger.h"

class WindowContainer;

class RenderWindow
{
public:
	bool Initialize( WindowContainer* pWindowContainer, HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int width, int height );
	bool ProcessMessages() noexcept;
	~RenderWindow() noexcept;
private:
	void RegisterWindowClass() noexcept;
	HWND hWnd = NULL;
	HINSTANCE hInstance = NULL;
	std::string windowTitle = "";
	std::wstring windowTitle_Wide = L"";
	std::string windowClass = "";
	std::wstring windowClass_Wide = L"";
	int width, height;
};
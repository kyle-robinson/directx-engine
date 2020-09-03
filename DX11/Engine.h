#pragma once
#include "WindowContainer.h"

class Engine : public WindowContainer
{
public:
	bool Initialize( HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int width, int height ) noexcept;
	bool ProcessMessages() noexcept;
};
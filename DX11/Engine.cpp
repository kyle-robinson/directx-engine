#include "Engine.h"

bool Engine::Initialize( HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int width, int height ) noexcept
{
	return this->renderWindow.Initialize( hInstance, windowTitle, windowClass, width, height );
}

bool Engine::ProcessMessages() noexcept
{
	return this->renderWindow.ProcessMessages();
}
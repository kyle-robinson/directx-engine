#include "Engine.h"
#include <string>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DirectXTK.lib" )

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	Engine engine;
	engine.Initialize( hInstance, "DirectX 11 Engine Window", "WindowClass", 1280, 720 );
	while ( engine.ProcessMessages() == true )
	{

	}
	return 0;
}
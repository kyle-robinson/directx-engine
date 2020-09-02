#include "ErrorLogger.h"
#include <comdef.h>

void ErrorLogger::Log( const std::string& message ) noexcept
{
	std::string errorMessage = "Error: " + message;
	MessageBoxA( NULL, errorMessage.c_str(), "Error", MB_ICONERROR );
}

void ErrorLogger::Log( HRESULT hr, const std::string& message ) noexcept
{
	_com_error error( hr );
	std::wstring errorMessage = L"Error: " + StringConverter::StringToWide( message ) + L"\n";
	MessageBoxW( NULL, errorMessage.c_str(), L"Error: ", MB_ICONERROR );
}
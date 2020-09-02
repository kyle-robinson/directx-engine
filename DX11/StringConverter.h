#pragma once
#include <string>

class StringConverter
{
public:
	static std::wstring StringToWide( const std::string& narrow ) noexcept;
	static std::string StringToNarrow( const std::wstring& wide ) noexcept;
};
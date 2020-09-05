#pragma once
#include "Exception.h"

#define RGC_EXCEPTION( message ) RGCompileException( ( message ), __LINE__, __FILE__ )

class RGCompileException : public Exception
{
public:
	RGCompileException( std::string message, int line, const char* file ) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetMessage() const noexcept;
private:
	std::string message;
};
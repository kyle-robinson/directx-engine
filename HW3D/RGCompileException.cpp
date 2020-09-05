#include "RGCompileException.h"
#include <sstream>

namespace Rgph
{
	RGCompileException::RGCompileException(std::string message, int line, const char* file) noexcept
		: Exception(line, file), message(std::move(message))
	{ }

	const char* RGCompileException::what() const noexcept
	{
		std::ostringstream oss;
		oss << Exception::what() << std::endl;
		oss << "[message]" << std::endl;
		oss << message;
		whatBuffer = oss.str();
		return whatBuffer.c_str();
	}

	const char* RGCompileException::GetType() const noexcept
	{
		return "Render Graph Compile Exception";
	}

	const std::string& RGCompileException::GetMessage() const noexcept
	{
		return message;
	}
}
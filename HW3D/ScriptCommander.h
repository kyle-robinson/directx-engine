#pragma once
#include "Exception.h"
#include <vector>
#include <string>

class ScriptCommander
{
public:
	class Completion : public Exception
	{
	public:
		Completion( const std::string& content ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
	private:
		std::string content;
	};
	class ScriptException : public Exception
	{
	public:
		ScriptException( int line, const char* file, const std::string& script = "", const std::string& message = "" ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
	private:
		std::string script;
		std::string message;
	};
public:
	ScriptCommander( const std::vector<std::string>& args );
private:
	void Publish( std::string path ) const;
};
#include "ScriptCommander.h"
#include "TexturePreprocessor.h"
#include "json/json.hpp"
#include <sstream>
#include <fstream>
#include <filesystem>

namespace json = nlohmann;
using namespace std::string_literals;

#define SCRIPT_ERROR( msg ) ScriptException( __LINE__, __FILE__, scriptPath,( msg ) )

ScriptCommander::ScriptCommander( const std::vector<std::string>& args )
{
	if( args.size() >= 2 && args[0] == "--commands" )
	{
		const auto scriptPath = args[1];
		std::ifstream script( scriptPath );
		if( !script.is_open() )
			throw SCRIPT_ERROR( "Unable to open script file"s );

		json::json top;
		script >> top;

		if( top.at( "enabled" ) )
		{
			bool abort = false;
			for( const auto& j : top.at( "commands" ) )
			{
				const auto commandName = j.at( "command" ).get<std::string>();
				const auto params = j.at( "params" );
				if( commandName == "flip-y" )
				{
					const auto source = params.at( "source" );
					TexturePreprocessor::FlipYNormalMap( source,params.value( "dest",source ) );
					abort = true;
				}
				else if( commandName == "flip-y-obj" )
				{
					TexturePreprocessor::FlipAllYNormalsInObj( params.at( "source" ) );
					abort = true;
				}
				else if( commandName == "validate-nmap" )
				{
					TexturePreprocessor::ValidateNormalMap( params.at( "source" ),params.at( "min" ),params.at( "max" ) );
					abort = true;
				}
				else if( commandName == "publish" )
				{
					Publish( params.at( "dest" ) );
					abort = true;
				}
				else
				{
					throw SCRIPT_ERROR( "Unknown command: "s + commandName );
				}
			}
			if( abort )
			{
				throw Completion( "Command(s) completed successfully" );
			}
		}
	}
}

void ScriptCommander::Publish( std::string path ) const
{
	namespace fs = std::filesystem;
	fs::create_directory( path );
	// copy executable
	fs::copy_file( R"(..\x64\Release\HW3D.exe)",path + R"(\HW3D.exe)",fs::copy_options::overwrite_existing );
	// copy assimp ini
	fs::copy_file( "imgui.ini",path + R"(\imgui.ini)",fs::copy_options::overwrite_existing );
	// copy all dlls
	for( auto& p : fs::directory_iterator( "" ) )
	{
		if( p.path().extension() == L".dll" )
		{
			fs::copy_file( p.path(),path + "\\" + p.path().filename().string(),
				fs::copy_options::overwrite_existing
			);
		}
	}
	// copy compiled shaders
	fs::copy( "res\\shaders\\cso",path + R"(\cso)",fs::copy_options::overwrite_existing );
	// copy assets
	fs::copy( "res\\textures",path + R"(\textures)",fs::copy_options::overwrite_existing | fs::copy_options::recursive );
	fs::copy( "res\\models",path + R"(\models)",fs::copy_options::overwrite_existing | fs::copy_options::recursive );
}

ScriptCommander::Completion::Completion( const std::string& content ) noexcept
	:
	Exception( 69,"@ScriptCommanderAbort" ),
	content( content )
{}

const char* ScriptCommander::Completion::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << std::endl << content;
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ScriptCommander::Completion::GetType() const noexcept
{
	return "Script Command Completed";
}

ScriptCommander::ScriptException::ScriptException( int line,const char* file,const std::string& script,const std::string& message ) noexcept
	:
	Exception( 69,"@ScriptCommanderAbort" ),
	script( script ),
	message( message )
{}

const char* ScriptCommander::ScriptException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Script File] " << script << std::endl
		<< message;
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ScriptCommander::ScriptException::GetType() const noexcept
{
	return "Script Command Error";
}
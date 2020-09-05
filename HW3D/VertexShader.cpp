#include "VertexShader.h"
#include "BindableCodex.h"
#include "StringConverter.h"
#include "GraphicsThrowMacros.h"
#include <typeinfo>
#include <d3dcompiler.h>

namespace Bind
{
	VertexShader::VertexShader( Graphics& gfx, const std::string& path ) : path( path )
	{
		INFOMANAGER( gfx );

		GFX_THROW_INFO( D3DReadFileToBlob( ToWide( path ).c_str(), &pBytecodeBlob));
		GFX_THROW_INFO( GetDevice( gfx )->CreateVertexShader(
			pBytecodeBlob->GetBufferPointer(),
			pBytecodeBlob->GetBufferSize(),
			nullptr,
			&pVertexShader
		) );
	}

	void VertexShader::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		INFOMANAGER( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->VSSetShader( pVertexShader.Get(), nullptr, 0u ) );
	}

	ID3DBlob* VertexShader::GetByteCode() const noexcept
	{
		return pBytecodeBlob.Get();
	}

	std::shared_ptr<VertexShader> VertexShader::Resolve( Graphics& gfx, const std::string& path )
	{
		return Codex::Resolve<VertexShader>( gfx, path );
	}

	std::string VertexShader::GenerateUID( const std::string& path )
	{
		using namespace std::string_literals;
		return typeid( VertexShader ).name() + "#"s + path;
	}

	std::string VertexShader::GetUID() const noexcept
	{
		return GenerateUID( path );
	}
}
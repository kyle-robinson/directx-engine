#include "PixelShader.h"
#include "BindableCodex.h"
#include "StringConverter.h"
#include "GraphicsThrowMacros.h"
#include <d3dcompiler.h>

namespace Bind
{
	PixelShader::PixelShader( Graphics& gfx, const std::string& path ) : path( path )
	{
		INFOMANAGER( gfx );

		GFX_THROW_INFO( D3DReadFileToBlob( ToWide( path ).c_str(), &pBytecodeBlob ) );
		GFX_THROW_INFO( GetDevice( gfx )->CreatePixelShader(
			pBytecodeBlob->GetBufferPointer(),
			pBytecodeBlob->GetBufferSize(),
			nullptr,
			&pPixelShader
		) );
	}

	void PixelShader::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		INFOMANAGER( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetShader( pPixelShader.Get(), nullptr, 0u ) );
	}

	ID3DBlob* PixelShader::GetByteCode() const noexcept
	{
		return pBytecodeBlob.Get();
	}

	std::shared_ptr<PixelShader> PixelShader::Resolve( Graphics& gfx, const std::string& path )
	{
		return Codex::Resolve<PixelShader>( gfx, "res\\shaders\\cso\\" + path );
	}

	std::string PixelShader::GenerateUID( const std::string& path )
	{
		using namespace std::string_literals;
		return typeid(PixelShader).name() + "#"s + path;
	}

	std::string PixelShader::GetUID() const noexcept
	{
		return GenerateUID( path );
	}
}
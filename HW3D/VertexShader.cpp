#include "VertexShader.h"
#include "GraphicsThrowMacros.h"
#include <d3dcompiler.h>

VertexShader::VertexShader( Graphics& gfx, const std::wstring& path )
{
	INFOMANAGER( gfx );

	GFX_THROW_INFO( D3DReadFileToBlob( path.c_str(), &pBytecodeBlob ) );
	GFX_THROW_INFO( GetDevice( gfx )->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	) );
}

void VertexShader::Bind( Graphics& gfx ) noexcept
{
	INFOMANAGER( gfx );
	GFX_THROW_INFO_ONLY( GetContext( gfx )->VSSetShader( pVertexShader.Get(), nullptr, 0u ) );
}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return pBytecodeBlob.Get();
}
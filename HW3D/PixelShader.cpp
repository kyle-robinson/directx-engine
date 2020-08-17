#include "PixelShader.h"
#include "GraphicsThrowMacros.h"
#include <d3dcompiler.h>

namespace Bind
{
	PixelShader::PixelShader( Graphics& gfx, const std::wstring& path )
	{
		INFOMANAGER( gfx );

		GFX_THROW_INFO( D3DReadFileToBlob( path.c_str(), &pBytecodeBlob ) );
		GFX_THROW_INFO( GetDevice( gfx )->CreatePixelShader(
			pBytecodeBlob->GetBufferPointer(),
			pBytecodeBlob->GetBufferSize(),
			nullptr,
			&pPixelShader
		) );
	}

	void PixelShader::Bind( Graphics& gfx ) noexcept
	{
		INFOMANAGER( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetShader( pPixelShader.Get(), nullptr, 0u ) );
	}

	ID3DBlob* PixelShader::GetByteCode() const noexcept
	{
		return pBytecodeBlob.Get();
	}
}
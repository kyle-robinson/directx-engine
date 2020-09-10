#include "NullPixelShader.h"
#include "BindableCodex.h"
#include "StringConverter.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	NullPixelShader::NullPixelShader( Graphics& gfx ) { }

	void NullPixelShader::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		INFOMANAGER_NOHR( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetShader( nullptr, nullptr, 0u ) );
	}

	std::shared_ptr<NullPixelShader> NullPixelShader::Resolve( Graphics& gfx )
	{
		return Codex::Resolve<NullPixelShader>( gfx );
	}

	std::string NullPixelShader::GenerateUID()
	{
		return typeid(NullPixelShader).name();
	}

	std::string NullPixelShader::GetUID() const noexcept
	{
		return GenerateUID();
	}
}
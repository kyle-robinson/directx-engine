#include "InputLayout.h"
#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	InputLayout::InputLayout( Graphics& gfx, VertexMeta::VertexLayout layout_in, ID3DBlob* pVertexShaderByteCode )
		: layout( std::move( layout_in ) )
	{
		INFOMANAGER( gfx );

		const auto d3dLayout = layout.GetD3DLayout();

		GFX_THROW_INFO( GetDevice( gfx )->CreateInputLayout(
			d3dLayout.data(),
			(UINT)d3dLayout.size(),
			pVertexShaderByteCode->GetBufferPointer(),
			pVertexShaderByteCode->GetBufferSize(),
			&pInputLayout
		) );
	}

	void InputLayout::Bind( Graphics& gfx ) noexcept
	{
		INFOMANAGER( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->IASetInputLayout( pInputLayout.Get() ) );
	}

	std::shared_ptr<InputLayout> InputLayout::Resolve( Graphics& gfx, const VertexMeta::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode )
	{
		return Codex::Resolve<InputLayout>( gfx, layout, pVertexShaderByteCode );
	}

	std::string InputLayout::GenerateUID( const VertexMeta::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode )
	{
		using namespace std::string_literals;
		return typeid( InputLayout ).name() + "#"s + layout.GetCode();
	}
}
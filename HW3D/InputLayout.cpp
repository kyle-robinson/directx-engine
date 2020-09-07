#include "InputLayout.h"
#include "VertexShader.h"
#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	InputLayout::InputLayout( Graphics& gfx, VertexMeta::VertexLayout layout_in, const VertexShader& vs )
		: layout( std::move( layout_in ) )
	{
		INFOMANAGER( gfx );

		GFX_THROW_INFO( GetDevice( gfx )->CreateInputLayout(
			layout_in.GetD3DLayout().data(),
			(UINT)layout_in.GetD3DLayout().size(),
			vs.GetByteCode()->GetBufferPointer(),
			vs.GetByteCode()->GetBufferSize(),
			&pInputLayout
		) );
	}

	void InputLayout::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		GFX_THROW_INFO_ONLY( GetContext( gfx )->IASetInputLayout( pInputLayout.Get() ) );
	}

	const VertexMeta::VertexLayout InputLayout::GetLayout() const noexcept
	{
		return layout;
	}

	std::shared_ptr<InputLayout> InputLayout::Resolve( Graphics& gfx, const VertexMeta::VertexLayout& layout, const VertexShader& vs )
	{
		return Codex::Resolve<InputLayout>( gfx, layout, vs );
	}

	std::string InputLayout::GenerateUID( const VertexMeta::VertexLayout& layout, const VertexShader& vs )
	{
		using namespace std::string_literals;
		return typeid( InputLayout ).name() + "#"s + layout.GetCode() + "#"s + vs.GetUID();
	}

	std::string InputLayout::GetUID() const noexcept
	{
		using namespace std::string_literals;
		return typeid( InputLayout ).name() + "#"s + layout.GetCode() + "#"s + vertexShaderUID;
	}
}
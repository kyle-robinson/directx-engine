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

		const auto d3dLayout = layout.GetD3DLayout();
		const auto pByteCode = vs.GetByteCode();

		GFX_THROW_INFO( GetDevice( gfx )->CreateInputLayout(
			d3dLayout.data(),
			(UINT)d3dLayout.size(),
			pByteCode->GetBufferPointer(),
			pByteCode->GetBufferSize(),
			&pInputLayout
		) );
	}

	void InputLayout::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		INFOMANAGER_NOHR( gfx );
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
#pragma once
#include "Vertex.h"
#include "Bindable.h"

namespace Bind
{
	class InputLayout : public Bindable
	{
	public:
		InputLayout( Graphics& gfx, VertexMeta::VertexLayout layout_in, ID3DBlob* pVertexShaderByteCode );
		void Bind( Graphics& gfx ) noexcept override;
		static std::shared_ptr<Bindable> Resolve( Graphics& gfx, const VertexMeta::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode );
		static std::string GenerateUID( const VertexMeta::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode = nullptr );
	protected:
		VertexMeta::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}
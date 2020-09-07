#pragma once
#include "Vertex.h"
#include "Bindable.h"

namespace Bind
{
	class VertexShader;

	class InputLayout : public Bindable
	{
	public:
		InputLayout( Graphics& gfx, VertexMeta::VertexLayout layout_in, const VertexShader& vs );
		void Bind( Graphics& gfx ) noexcept(!IS_DEBUG) override;
		const VertexMeta::VertexLayout GetLayout() const noexcept;
		static std::shared_ptr<InputLayout> Resolve( Graphics& gfx, const VertexMeta::VertexLayout& layout, const VertexShader& vs );
		static std::string GenerateUID( const VertexMeta::VertexLayout& layout, const VertexShader& vs );
		std::string GetUID() const noexcept override;
	protected:
		std::string vertexShaderUID;
		VertexMeta::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}
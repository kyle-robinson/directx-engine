#include "FullscreenPass.h"
#include "BindableCommon.h"

namespace Rgph
{
	FullscreenPass::FullscreenPass(const std::string& name, Graphics& gfx) noexcept(!IS_DEBUG) :
		BindingPass(std::move(name))
	{
		// setup geometry
		VertexMeta::VertexLayout layout;
		layout.Append(VertexMeta::VertexLayout::Position2D);
		VertexMeta::VertexBuffer bufFull{ layout };
		bufFull.EmplaceBack(DirectX::XMFLOAT2{ -1,  1 });
		bufFull.EmplaceBack(DirectX::XMFLOAT2{ 1,  1 });
		bufFull.EmplaceBack(DirectX::XMFLOAT2{ -1, -1 });
		bufFull.EmplaceBack(DirectX::XMFLOAT2{ 1, -1 });
		AddBind(Bind::VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull)));
		std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
		AddBind(Bind::IndexBuffer::Resolve(gfx, "$Full", std::move(indices)));

		// setup other common fullscreen bindables
		auto vs = Bind::VertexShader::Resolve(gfx, "Fullscreen_VS.cso");
		AddBind(Bind::InputLayout::Resolve(gfx, layout, vs->GetByteCode()));
		AddBind(std::move(vs));
		AddBind(Bind::Topology::Resolve(gfx));
		AddBind(Bind::Rasterizer::Resolve(gfx, false));
	}

	void FullscreenPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		BindAll(gfx);
		gfx.DrawIndexed(6u);
	}
}
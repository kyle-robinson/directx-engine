#include "HorizontalBlurPass.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "Sink.h"
#include "Source.h"
#include "Blender.h"

namespace Rgph
{
	HorizontalBlurPass::HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int width, unsigned int height) :
		FullscreenPass(std::move(name), gfx)
	{
		AddBind(Bind::PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
		AddBind(Bind::Blender::Resolve(gfx, false));

		RegisterSink(DirectBindableSink<Bind::Bindable>::Make("control", control));
		RegisterSink(DirectBindableSink<Bind::CachingPixelConstantBufferEx>::Make("direction", direction));
		RegisterSink(DirectBindableSink<Bind::Bindable>::Make("scratchIn", blurScratchIn));

		renderTarget = std::make_shared<Bind::ShaderInputRenderTarget>(gfx, width / 2, height / 2, 0u);
		RegisterSource(DirectBindableSource<Bind::RenderTarget>::Make("scratchOut", renderTarget));
	}

	void HorizontalBlurPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = true;
		direction->SetBuffer(buf);

		blurScratchIn->Bind(gfx);
		control->Bind(gfx);
		direction->Bind(gfx);
		FullscreenPass::Execute(gfx);
	}
}
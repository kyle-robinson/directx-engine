#include "HorizontalBlurPass.h"
#include "ConstantBufferEx.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "Sink.h"
#include "Source.h"
#include "Blender.h"
#include "Sampler.h"

namespace Rgph
{
	HorizontalBlurPass::HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int width, unsigned int height) :
		FullscreenPass(std::move(name), gfx)
	{
		AddBind(Bind::PixelShader::Resolve(gfx, "BlurOutlinePS.cso"));
		AddBind(Bind::Blender::Resolve(gfx, false));
		AddBind(Bind::Sampler::Resolve(gfx, Bind::Sampler::Type::Point, true));

		AddBindSink<Bind::RenderTarget>( "scratchIn" );
		AddBindSink<Bind::CachingPixelConstantBufferEx>( "kernel" );
		RegisterSink( DirectBindableSink<Bind::CachingPixelConstantBufferEx>::Make( "direction", direction ) );

		renderTarget = std::make_shared<Bind::ShaderInputRenderTarget>(gfx, width / 2, height / 2, 0u);
		RegisterSource(DirectBindableSource<Bind::RenderTarget>::Make("scratchOut", renderTarget));
	}

	void HorizontalBlurPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = true;
		direction->SetBuffer(buf);

		direction->Bind(gfx);
		FullscreenPass::Execute(gfx);
	}
}
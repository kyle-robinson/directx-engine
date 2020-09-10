#pragma once
#include "RenderQueuePass.h"
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include "Camera.h"
#include "Job.h"
#include <vector>

class Graphics;

namespace Rgph
{
	class LambertianPass : public RenderQueuePass
	{
	public:
		void BindMainCamera( const Camera& cam ) noexcept
		{
			pMainCamera = &cam;
		}
		LambertianPass(Graphics& gfx, std::string name) : RenderQueuePass(std::move(name))
		{
			RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Off));
		}
		void Execute( Graphics& gfx ) const noexcept(!IS_DEBUG) override
		{
			assert( pMainCamera );
			pMainCamera->BindToGraphics( gfx );
			RenderQueuePass::Execute( gfx );
		}
	private:
		const Camera* pMainCamera = nullptr;
	};
}
#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include "Sink.h"
#include "Source.h"
#include "Camera.h"
#include "Stencil.h"
#include "DepthStencil.h"
#include "ShadowCameraCbuf.h"
#include "ShadowSampler.h"
#include <vector>

class Graphics;

namespace Rgph
{
	class LambertianPass : public RenderQueuePass
	{
	public:
		LambertianPass(Graphics& gfx, std::string name) :
			RenderQueuePass(std::move(name)),
			pShadowCbuf{ std::make_shared<Bind::ShadowCameraCbuf>( gfx ) },
			pShadowSampler{ std::make_shared<Bind::ShadowSampler>( gfx ) }
		{
			AddBind( pShadowCbuf );
			AddBind( pShadowSampler );
			RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBindSink<Bind::Bindable>( "shadowMap" );
			AddBindSink<Bind::Bindable>( "shadowControl" );
			RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Off));
		}
		void BindMainCamera( const Camera& cam ) noexcept
		{
			pMainCamera = &cam;
		}
		void BindShadowCamera( const Camera& cam ) noexcept
		{
			pShadowCbuf->SetCamera( &cam );
		}
		void Execute( Graphics& gfx ) const noexcept(!IS_DEBUG) override
		{
			assert( pMainCamera );
			pShadowCbuf->Update( gfx );
			pMainCamera->BindToGraphics( gfx );
			RenderQueuePass::Execute( gfx );
		}
	private:
		std::shared_ptr<Bind::ShadowSampler> pShadowSampler;
		std::shared_ptr<Bind::ShadowCameraCbuf> pShadowCbuf;
		const Camera* pMainCamera = nullptr;
	};
}
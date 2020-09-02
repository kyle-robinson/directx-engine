#pragma once
#include "BindableCommon.h"
#include "DepthStencil.h"
#include "Graphics.h"
#include "Pass.h"
#include "Job.h"
#include <array>

class FrameCommander
{
public:
	FrameCommander( Graphics& gfx ) : ds( gfx, gfx.GetWidth(), gfx.GetHeight() ) {}
	void Accept( Job job, size_t target ) noexcept
	{
		passes[target].Accept( job );
	}
	void Execute( Graphics& gfx ) const noexcept(!IS_DEBUG)
	{
		// setup render target
		ds.Clear( gfx );
		gfx.BindSwapBuffer( ds );

		// main lighting pass
		Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Off )->Bind( gfx );
		passes[0].Execute( gfx );

		// outline masking pass
		Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Write )->Bind( gfx );
		Bind::NullPixelShader::Resolve( gfx )->Bind( gfx );
		passes[1].Execute( gfx );

		// outline drawing pass
		Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Mask )->Bind( gfx );
		passes[2].Execute( gfx );
	}
	void Reset() noexcept
	{
		for ( auto& p : passes )
			p.Reset();
	}
private:
	std::array<Pass, 3> passes;
	DepthStencil ds;
};
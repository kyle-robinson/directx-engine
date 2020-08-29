#pragma once
#include "BindableCommon.h"
#include "Graphics.h"
#include "Pass.h"
#include "Job.h"
#include <array>

class FrameCommander
{
public:
	void Accept( Job job, size_t target ) noexcept
	{
		passes[target].Accept( job );
	}
	void Execute( Graphics& gfx ) const noexcept(!IS_DEBUG)
	{
		// main lighting pass
		Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Off )->Bind( gfx );
		passes[0].Execute( gfx );

		// outline masking pass
		Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Write )->Bind( gfx );
		Bind::NullPixelShader::Resolve( gfx )->Bind( gfx );
		passes[1].Execute( gfx );

		// outline drawing pass
		Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Mask )->Bind( gfx );
		struct SolidCBuf
		{
			DirectX::XMFLOAT4 color = { 1.0f, 0.4f, 0.4f, 1.0f };
		} scb;
		Bind::PixelConstantBuffer<SolidCBuf>::Resolve( gfx, scb, 1u )->Bind( gfx );
		passes[2].Execute( gfx );
	}
	void Reset() noexcept
	{
		for ( auto& p : passes )
			p.Reset();
	}
private:
	std::array<Pass, 3> passes;
};
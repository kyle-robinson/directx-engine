#pragma once
#include "BindableCommon.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Graphics.h"
#include "Blurring.h"
#include "Pass.h"
#include "Job.h"
#include <array>

class FrameCommander
{
public:
	FrameCommander( Graphics& gfx ) :
		ds( gfx, gfx.GetWidth(), gfx.GetHeight() ),
		rt1( gfx, gfx.GetWidth(), gfx.GetHeight() ),
		rt2( gfx, gfx.GetWidth(), gfx.GetHeight() ),
		blur( gfx, 7, 2.6f, "BlurOutlinePS.cso" )
	{
		// setup fullscreen geometry
		VertexMeta::VertexLayout layout;
		layout.Append( VertexMeta::VertexLayout::Position2D );
		VertexMeta::VertexBuffer buffer{ layout };
		buffer.EmplaceBack( DirectX::XMFLOAT2{ -1,  1 } );
		buffer.EmplaceBack( DirectX::XMFLOAT2{  1,  1 } );
		buffer.EmplaceBack( DirectX::XMFLOAT2{ -1, -1 } );
		buffer.EmplaceBack( DirectX::XMFLOAT2{  1, -1 } );
		pVBufferFull = Bind::VertexBuffer::Resolve( gfx, "$Full", std::move( buffer ) );
		std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
		pIBufferFull = Bind::IndexBuffer::Resolve( gfx, "$Full", std::move( indices ) );

		// setup framebuffer shaders
		pVShaderFull = Bind::VertexShader::Resolve( gfx, "FullscreenVS.cso" );
		pILayoutFull = Bind::InputLayout::Resolve( gfx, layout, pVShaderFull->GetByteCode() );
		pSamplerFull = Bind::Sampler::Resolve( gfx, false, true );
		pBlenderMerge = Bind::Blender::Resolve( gfx, true );
	}
	void Accept( Job job, size_t target ) noexcept
	{
		passes[target].Accept( job );
	}
	void Execute( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		// setup render target
		ds.Clear( gfx );
		rt1.Clear( gfx );
		gfx.BindSwapBuffer( ds );

		// main lighting pass
		Bind::Blender::Resolve( gfx, false )->Bind( gfx );
		Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Off )->Bind( gfx );
		passes[0].Execute( gfx );

		// outline masking pass
		Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Write )->Bind( gfx );
		Bind::NullPixelShader::Resolve( gfx )->Bind( gfx );
		passes[1].Execute( gfx );

		// outline drawing pass
		rt1.BindAsTarget( gfx );
		Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Off )->Bind( gfx );
		passes[2].Execute( gfx );

		// framebuffer h-pass
		rt2.BindAsTarget( gfx );
		rt1.BindAsTexture( gfx, 0 );

		pVBufferFull->Bind( gfx );
		pIBufferFull->Bind( gfx );
		pVShaderFull->Bind( gfx );
		pILayoutFull->Bind( gfx );
		pSamplerFull->Bind( gfx );

		blur.Bind( gfx );
		blur.SetHorizontal( gfx );
		gfx.DrawIndexed( pIBufferFull->GetCount() );

		// framebuffer v-pass
		gfx.BindSwapBuffer( ds );
		rt2.BindAsTexture( gfx, 0u );
		pBlenderMerge->Bind( gfx );
		Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Mask )->Bind( gfx );
		blur.SetVertical( gfx );
		gfx.DrawIndexed( pIBufferFull->GetCount() );
	}
	void Reset() noexcept
	{
		for ( auto& p : passes )
			p.Reset();
	}
private:
	std::array<Pass, 3> passes;
	DepthStencil ds;
	RenderTarget rt1;
	RenderTarget rt2;
	Blurring blur;
	std::shared_ptr<Bind::VertexBuffer> pVBufferFull;
	std::shared_ptr<Bind::IndexBuffer> pIBufferFull;
	std::shared_ptr<Bind::VertexShader> pVShaderFull;
	std::shared_ptr<Bind::InputLayout> pILayoutFull;
	std::shared_ptr<Bind::Sampler> pSamplerFull;
	std::shared_ptr<Bind::Blender> pBlenderMerge;
};
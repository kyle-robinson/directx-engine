#pragma once
#include "BindableCommon.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Graphics.h"
#include "Pass.h"
#include "Job.h"
#include <array>

class FrameCommander
{
public:
	FrameCommander( Graphics& gfx )
		: ds( gfx, gfx.GetWidth(), gfx.GetHeight() ), rt( gfx, gfx.GetWidth(), gfx.GetHeight() )
	{
		// setup fullscreen geometry
		VertexMeta::VertexLayout layout;
		layout.Append( VertexMeta::VertexLayout::Position2D );
		VertexMeta::VertexBuffer buffer{ std::move( layout ) };
		buffer.EmplaceBack( DirectX::XMFLOAT2{ -1,  1 } );
		buffer.EmplaceBack( DirectX::XMFLOAT2{  1,  1 } );
		buffer.EmplaceBack( DirectX::XMFLOAT2{ -1, -1 } );
		buffer.EmplaceBack( DirectX::XMFLOAT2{  1, -1 } );
		pVBufferFull = Bind::VertexBuffer::Resolve( gfx, "$Full", std::move( buffer ) );
		std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
		pIBufferFull = Bind::IndexBuffer::Resolve( gfx, "$Full", std::move( indices ) );

		// setup framebuffer shaders
		pVShaderFull = Bind::VertexShader::Resolve( gfx, "FullscreenVS.cso" );
		pPShaderFull = Bind::PixelShader::Resolve( gfx, "FullscreenPS.cso" );
		pILayoutFull = Bind::InputLayout::Resolve( gfx, layout, pVShaderFull->GetByteCode() );
	}
	void Accept( Job job, size_t target ) noexcept
	{
		passes[target].Accept( job );
	}
	void Execute( Graphics& gfx ) const noexcept(!IS_DEBUG)
	{
		// setup render target
		ds.Clear( gfx );
		rt.BindAsTarget( gfx, ds );

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

		// framebuffer pass
		gfx.BindSwapBuffer();
		rt.BindAsTexture( gfx, 0 );
		pVBufferFull->Bind( gfx );
		pIBufferFull->Bind( gfx );
		pVShaderFull->Bind( gfx );
		pPShaderFull->Bind( gfx );
		pILayoutFull->Bind( gfx );
		gfx.DrawIndexed( pIBufferFull->GetCount() );
	}
	void Reset() noexcept
	{
		for ( auto& p : passes )
			p.Reset();
	}
private:
	static VertexMeta::VertexLayout MakeFullscreenQuadLayout()
	{
		VertexMeta::VertexLayout layout;
		layout.Append( VertexMeta::VertexLayout::Position2D );
		return layout;
	}
private:
	std::array<Pass, 3> passes;
	DepthStencil ds;
	RenderTarget rt;
	std::shared_ptr<Bind::VertexBuffer> pVBufferFull;
	std::shared_ptr<Bind::IndexBuffer> pIBufferFull;
	std::shared_ptr<Bind::VertexShader> pVShaderFull;
	std::shared_ptr<Bind::PixelShader> pPShaderFull;
	std::shared_ptr<Bind::InputLayout> pILayoutFull;
};
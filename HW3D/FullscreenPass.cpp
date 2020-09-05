#include "FullscreenPass.h"
#include "BindableCommon.h"

FullscreenPass::FullscreenPass( const std::string& name, Graphics& gfx ) noexcept(!IS_DEBUG) :
	BindingPass( std::move( name ) )
{
	// static initialization if required
	if ( !indexBuffer )
	{
		// setup geometry
		VertexMeta::VertexLayout layout;
		layout.Append( VertexMeta::VertexLayout::Position2D );
		VertexMeta::VertexBuffer buffer{ layout };
		buffer.EmplaceBack( DirectX::XMFLOAT2{ -1,  1} );
		buffer.EmplaceBack( DirectX::XMFLOAT2{  1,  1} );
		buffer.EmplaceBack( DirectX::XMFLOAT2{ -1, -1} );
		buffer.EmplaceBack( DirectX::XMFLOAT2{  1, -1} );
		vertexBuffer = Bind::VertexBuffer::Resolve( gfx, "$Full", std::move( buffer ) );
		std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
		indexBuffer = Bind::IndexBuffer::Resolve( gfx, "$Full", std::move( indices ) );
		
		// setup shaders
		vertexShader = Bind::VertexShader::Resolve( gfx, "FullscreenVS.cso" );
		inputLayout = Bind::InputLayout::Resolve( gfx, layout, vertexShader->GetByteCode() );
	}
}

void FullscreenPass::Execute( Graphics& gfx ) const noexcept
{
	BindAll( gfx );
	vertexBuffer->Bind( gfx );
	indexBuffer->Bind( gfx );
	vertexBuffer->Bind( gfx );
	inputLayout->Bind( gfx );

	gfx.DrawIndexed( indexBuffer->GetCount() );
}

std::shared_ptr<Bind::VertexBuffer> FullscreenPass::vertexBuffer;
std::shared_ptr<Bind::IndexBuffer> FullscreenPass::indexBuffer;
std::shared_ptr<Bind::VertexShader> FullscreenPass::vertexShader;
std::shared_ptr<Bind::InputLayout> FullscreenPass::inputLayout;
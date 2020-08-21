#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>

void Drawable::Draw( Graphics& gfx ) const noexcept(!IS_DEBUG)
{
	for ( auto& b : binds )
	{
		b->Bind( gfx );
	}
	gfx.DrawIndexed( pIndexBuffer->GetCount() );
}

void Drawable::AddBind( std::shared_ptr<Bind::Bindable> bind ) noexcept(!IS_DEBUG)
{
	if ( typeid( *bind ) == typeid( Bind::IndexBuffer ) )
	{
		assert( "Tried to bind multiple index buffers!" && pIndexBuffer == nullptr );
		pIndexBuffer = &static_cast<Bind::IndexBuffer&>(*bind);
	}
	binds.push_back( std::move( bind ) );
}
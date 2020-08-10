#include "Topology.h"
#include "GraphicsThrowMacros.h"

Topology::Topology( Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type ) : type( type ) { }

void Topology::Bind( Graphics& gfx ) noexcept
{
	INFOMANAGER( gfx );

	GFX_THROW_INFO_ONLY( GetContext( gfx )->IASetPrimitiveTopology( type ) );
}
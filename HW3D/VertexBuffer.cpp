#include "VertexBuffer.h"

namespace Bind
{
	void VertexBuffer::Bind( Graphics& gfx ) noexcept
	{
		const UINT offset = 0u;
		INFOMANAGER( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->IASetVertexBuffers( 0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset ) );
	}
}
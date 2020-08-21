#include "VertexBuffer.h"

namespace Bind
{
	VertexBuffer::VertexBuffer( Graphics& gfx, const VertexMeta::VertexBuffer& vbuf ) : stride( (UINT)vbuf.GetLayout().Size() )
	{
		INFOMANAGER(gfx);

		D3D11_BUFFER_DESC bd = { 0 };
		bd.ByteWidth = UINT(vbuf.SizeBytes());
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.StructureByteStride = stride;

		D3D11_SUBRESOURCE_DATA sd = { 0 };
		sd.pSysMem = vbuf.GetData();

		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
	}

	void VertexBuffer::Bind( Graphics& gfx ) noexcept
	{
		const UINT offset = 0u;
		INFOMANAGER( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->IASetVertexBuffers( 0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset ) );
	}
}
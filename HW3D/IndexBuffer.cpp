#include "IndexBuffer.h"
#include "GraphicsThrowMacros.h"

IndexBuffer::IndexBuffer( Graphics& gfx, const std::vector<unsigned short>& indices ) : count( (UINT)indices.size() )
{
	INFOMANAGER( gfx );

	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.ByteWidth = UINT( count * sizeof( unsigned short ) );
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.StructureByteStride = sizeof( unsigned short );

	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = indices.data();
	GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &ibd, &isd, &pIndexBuffer ) );
}

void IndexBuffer::Bind( Graphics& gfx ) noexcept
{
	INFOMANAGER( gfx );
	GFX_THROW_INFO_ONLY( GetContext( gfx )->IASetIndexBuffer( pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u ) );
}

UINT IndexBuffer::GetCount() const noexcept
{
	return count;
}
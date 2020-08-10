#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer( Graphics& gfx, const std::vector<V>& vertices ) : stride( sizeof( V ) )
	{
		INFOMANAGER( gfx );

		D3D11_BUFFER_DESC bd = { 0 };
		bd.ByteWidth = UINT( sizeof( V ) * vertices.size() );
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.StructureByteStride = sizeof( V );

		D3D11_SUBRESOURCE_DATA sd = { 0 };
		sd.pSysMem = vertices.data();
		GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &bd, &sd, &pVertexBuffer ) );
	}
	void Bind( Graphics& gfx ) noexcept override;
protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};
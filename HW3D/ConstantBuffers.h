#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

template<typename C>
class ConstantBuffer : public Bindable
{
public:
	void Update( Graphics& gfx, const C& consts )
	{
		INFOMANAGER( gfx );

		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO( GetContext( gfx )->Map( pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr ) );
		memcpy( msr.pData, &consts, sizeof( consts ) );
		GetContext( gfx )->Unmap( pConstantBuffer.Get(), 0u );
	}
	ConstantBuffer( Graphics& gfx, const C& consts )
	{
		INFOMANAGER( gfx );

		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.ByteWidth = sizeof( consts );
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = { 0 };
		csd.pSysMem = &consts;
		GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &cbd, &csd, &pConstantBuffer ) );
	}
	ConstantBuffer(Graphics& gfx)
	{
		INFOMANAGER( gfx );

		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.ByteWidth = sizeof( C );
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.StructureByteStride = 0u;
		GetDevice( gfx )->CreateBuffer( &cbd, nullptr, &pConstantBuffer );
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind( Graphics& gfx ) noexcept override
	{
		GetContext( gfx )->VSSetConstantBuffers( 0u, 1u, pConstantBuffer.GetAddressOf() );
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind( Graphics& gfx ) noexcept override
	{
		GetContext( gfx )->PSSetConstantBuffers( 0u, 1u, pConstantBuffer.GetAddressOf() );
	}
};
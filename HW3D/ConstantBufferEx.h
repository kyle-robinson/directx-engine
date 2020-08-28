#pragma once
#include "Bindable.h"
#include "DynamicConstant.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	class PixelConstantBufferEx : public Bindable
	{
	public:
		void Update( Graphics& gfx, const DCB::Buffer& buf )
		{
			INFOMANAGER( gfx );
			assert( &buf.GetRootLayoutElement() == &GetRootLayoutElement() );

			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO( GetContext( gfx )->Map(
				pConstantBuffer.Get(),
				0u,
				D3D11_MAP_WRITE_DISCARD,
				0u,
				&msr
			) );
			memcpy( msr.pData, buf.GetData(), buf.GetSizeInBytes() );
			GetContext( gfx )->Unmap( pConstantBuffer.Get(), 0u );
		}
		void Bind( Graphics& gfx ) noexcept override
		{
			GetContext( gfx )->PSSetConstantBuffers( slot, 1u, pConstantBuffer.GetAddressOf() );
		}
		virtual const DCB::LayoutElement& GetRootLayoutElement() const noexcept = 0;
	protected:
		PixelConstantBufferEx(Graphics& gfx, const DCB::LayoutElement& layoutRoot, UINT slot, const DCB::Buffer* pBuf)
			:
			slot(slot)
		{
			INFOMANAGER( gfx );

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
			cbd.StructureByteStride = 0u;

			if (pBuf != nullptr)
			{
				D3D11_SUBRESOURCE_DATA csd = {};
				csd.pSysMem = pBuf->GetData();
				GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &cbd, &csd, &pConstantBuffer ) );
			}
			else
			{
				GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &cbd, nullptr, &pConstantBuffer ) );
			}
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot;
	};

	class CachingPixelConstantBufferEx : public PixelConstantBufferEx
	{
	public:
		CachingPixelConstantBufferEx(Graphics& gfx, const DCB::CompleteLayout& layout, UINT slot)
			:
			PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr),
			buf(DCB::Buffer(layout))
		{}
		CachingPixelConstantBufferEx(Graphics& gfx, const DCB::Buffer& buf, UINT slot)
			:
			PixelConstantBufferEx(gfx, buf.GetRootLayoutElement(), slot, &buf),
			buf(buf)
		{}
		const DCB::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return buf.GetRootLayoutElement();
		}
		const DCB::Buffer& GetBuffer() const noexcept
		{
			return buf;
		}
		void SetBuffer(const DCB::Buffer& buf_in)
		{
			buf.CopyFrom(buf_in);
			bufferSet = true;
		}
		void Bind(Graphics& gfx) noexcept override
		{
			if (bufferSet)
			{
				Update(gfx, buf);
				bufferSet = false;
			}
			PixelConstantBufferEx::Bind(gfx);
		}
	private:
		bool bufferSet = false;
		DCB::Buffer buf;
	};

	class NoCachePixelConstantBufferEx : public PixelConstantBufferEx
	{
	public:
		NoCachePixelConstantBufferEx(Graphics& gfx, const DCB::CompleteLayout& layout, UINT slot)
			:
			PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr),
			pLayoutRoot(layout.ShareRoot())
		{}
		NoCachePixelConstantBufferEx(Graphics& gfx, const DCB::Buffer& buf, UINT slot)
			:
			PixelConstantBufferEx(gfx, buf.GetRootLayoutElement(), slot, &buf),
			pLayoutRoot(buf.ShareLayoutRoot())
		{}
		const DCB::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return *pLayoutRoot;
		}
	private:
		std::shared_ptr<DCB::LayoutElement> pLayoutRoot;
	};
}
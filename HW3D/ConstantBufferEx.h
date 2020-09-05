#pragma once
#include "Bindable.h"
#include "DynamicConstant.h"
#include "GraphicsThrowMacros.h"
#include "TechniqueProbe.h"

namespace Bind
{
	class ConstantBufferEx : public Bindable
	{
	public:
		void Update( Graphics& gfx, const Dcb::Buffer& buf )
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
		virtual const Dcb::LayoutElement& GetRootLayoutElement() const noexcept = 0;
	protected:
		ConstantBufferEx(Graphics& gfx, const Dcb::LayoutElement& layoutRoot, UINT slot, const Dcb::Buffer* pBuf)
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
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot;
	};

	class PixelConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;
		void Bind( Graphics& gfx ) noexcept(!IS_DEBUG) override
		{
			INFOMANAGER_NOHR( gfx );
			GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetConstantBuffers( slot, 1u, pConstantBuffer.GetAddressOf() ) );
		}
	};

	class VertexConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override
		{
			INFOMANAGER_NOHR( gfx );
			GFX_THROW_INFO_ONLY( GetContext(gfx)->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf()) );
		}
	};

	template<class T>
	class CachingConstantBufferEx : public T
	{
	public:
		CachingConstantBufferEx(Graphics& gfx, const Dcb::CompleteLayout& layout, UINT slot)
			:
			T(gfx, *layout.ShareRoot(), slot, nullptr),
			buf(Dcb::Buffer(layout))
		{}
		CachingConstantBufferEx(Graphics& gfx, const Dcb::Buffer& buf, UINT slot)
			:
			T(gfx, buf.GetRootLayoutElement(), slot, &buf),
			buf(buf)
		{}
		const Dcb::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return buf.GetRootLayoutElement();
		}
		const Dcb::Buffer& GetBuffer() const noexcept
		{
			return buf;
		}
		void SetBuffer(const Dcb::Buffer& buf_in)
		{
			buf.CopyFrom(buf_in);
			bufferSet = true;
		}
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override
		{
			if (bufferSet)
			{
				T::Update(gfx, buf);
				bufferSet = false;
			}
			T::Bind(gfx);
		}
		void Accept( TechniqueProbe& probe ) override
		{
			if ( probe.VisitBuffer( buf ) )
				bufferSet = true;
		}
	private:
		bool bufferSet = false;
		Dcb::Buffer buf;
	};

	using CachingPixelConstantBufferEx = CachingConstantBufferEx<PixelConstantBufferEx>;
	using CachingVertexConstantBufferEx = CachingConstantBufferEx<VertexConstantBufferEx>;

	/*class NoCachePixelConstantBufferEx : public PixelConstantBufferEx
	{
	public:
		NoCachePixelConstantBufferEx(Graphics& gfx, const Dcb::CompleteLayout& layout, UINT slot)
			:
			PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr),
			pLayoutRoot(layout.ShareRoot())
		{}
		NoCachePixelConstantBufferEx(Graphics& gfx, const Dcb::Buffer& buf, UINT slot)
			:
			PixelConstantBufferEx(gfx, buf.GetRootLayoutElement(), slot, &buf),
			pLayoutRoot(buf.ShareLayoutRoot())
		{}
		const Dcb::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return *pLayoutRoot;
		}
	private:
		std::shared_ptr<Dcb::LayoutElement> pLayoutRoot;
	};*/
}
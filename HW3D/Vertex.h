#pragma once
#include "Graphics.h"
#include "Color.h"
#include <type_traits>
#include <vector>

namespace VertexMeta
{
	class VertexLayout
	{
	public:
		enum ElementType
		{
			Position2D,
			Position3D,
			Texture2D,
			Normal,
			Tangent,
			Bitangent,
			Float3Color,
			Float4Color,
			BGRAColor,
			Count
		};

		template<ElementType> struct Map;
		template<> struct Map<Position2D>
		{
			using VertexType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
		};
		template<> struct Map<Position3D>
		{
			using VertexType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
		};
		template<> struct Map<Texture2D>
		{
			using VertexType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "T2";
		};
		template<> struct Map<Normal>
		{
			using VertexType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N";
		};
		template<> struct Map<Tangent>
		{
			using VertexType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "Nt";
		};
		template<> struct Map<Bitangent>
		{
			using VertexType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Bitangent";
			static constexpr const char* code = "Nb";
		};
		template<> struct Map<Float3Color>
		{
			using VertexType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C3";
		};
		template<> struct Map<Float4Color>
		{
			using VertexType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C4";
		};
		template<> struct Map<BGRAColor>
		{
			using VertexType = ::BGRAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C8";
		};

		class Element
		{
		public:
			Element( ElementType type, size_t offset );
			size_t GetOffsetAfter() const noexcept(!IS_DEBUG);
			size_t GetOffset() const;
			size_t Size() const noexcept(!IS_DEBUG);
			ElementType GetType() const noexcept;
			static constexpr size_t SizeOf( ElementType type ) noexcept(!IS_DEBUG);
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept(!IS_DEBUG);
			const char* GetCode() const noexcept;
		private:
			template<ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc( size_t offset ) noexcept
			{
				return { Map<type>::semantic, 0, Map<type>::dxgiFormat, 0, (UINT)offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}
		private:
			ElementType type;
			size_t offset;
		};
	public:
		template<ElementType Type>
		const Element& Resolve() const noexcept(!IS_DEBUG)
		{
			for ( auto& e : elements )
			{
				if ( e.GetType() == Type )
					return e;
			}
			assert( "Could not resolve element type!"  && false );
			return elements.front();
		}
		const Element& ResolveByIndex( size_t i ) const noexcept(!IS_DEBUG);
		VertexLayout& Append( ElementType type ) noexcept(!IS_DEBUG);
		size_t Size() const noexcept(!IS_DEBUG);
		size_t GetElementCount() const noexcept;
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept(!IS_DEBUG);
		std::string GetCode() const noexcept(!IS_DEBUG);
	private:
		std::vector<Element> elements;
	};

	class Vertex
	{
		friend class VertexBuffer;
	public:
		template<VertexLayout::ElementType Type>
		auto& Attr() noexcept(!IS_DEBUG)
		{
			auto pAttribute = pData + layout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::VertexType*>( pAttribute );
		}
		template<typename T>
		void SetAttributeByIndex( size_t i, T&& value ) noexcept(!IS_DEBUG)
		{
			const auto& element = layout.ResolveByIndex( i );
			auto pAttribute = pData + element.GetOffset();
			switch ( element.GetType() )
			{
			case VertexLayout::Position2D:
				SetAttribute<VertexLayout::Position2D>( pAttribute, std::forward<T>( value ) );
				break;
			case VertexLayout::Position3D:
				SetAttribute<VertexLayout::Position3D>( pAttribute, std::forward<T>( value ) );
				break;
			case VertexLayout::Texture2D:
				SetAttribute<VertexLayout::Texture2D>( pAttribute, std::forward<T>( value ) );
				break;
			case VertexLayout::Normal:
				SetAttribute<VertexLayout::Normal>( pAttribute, std::forward<T>( value ) );
				break;
			case VertexLayout::Tangent:
				SetAttribute<VertexLayout::Tangent>( pAttribute, std::forward<T>( value ) );
				break;
			case VertexLayout::Bitangent:
				SetAttribute<VertexLayout::Bitangent>( pAttribute, std::forward<T>( value ) );
				break;
			case VertexLayout::Float3Color:
				SetAttribute<VertexLayout::Float3Color>( pAttribute, std::forward<T>( value ) );
				break;
			case VertexLayout::Float4Color:
				SetAttribute<VertexLayout::Float4Color>( pAttribute, std::forward<T>( value ) );
				break;
			case VertexLayout::BGRAColor:
				SetAttribute<VertexLayout::BGRAColor>( pAttribute, std::forward<T>( value ) );
				break;
			default:
				assert( "Bad element type!"  && false );
			}
		}
	protected:
		Vertex( char* pData, const VertexLayout& layout ) noexcept(!IS_DEBUG);
	private:
		// enables setting of multiple parameters in the parameter pack via element index
		template<typename First, typename ...Rest>
		void SetAttributeByIndex( size_t i, First&& first, Rest&&... rest ) noexcept(!IS_DEBUG)
		{
			SetAttributeByIndex( i, std::forward<First>( first ) );
			SetAttributeByIndex( i + 1, std::forward<Rest>( rest )... );
		}
		// helper function to reduce code duplication in SetIndexByAttribute
		template<VertexLayout::ElementType DestLayoutType, typename SrcType>
		void SetAttribute( char* pAttribute, SrcType&& value ) noexcept(!IS_DEBUG)
		{
			using Dest = typename VertexLayout::Map<DestLayoutType>::VertexType;
			if constexpr( std::is_assignable<Dest, SrcType>::value )
				*reinterpret_cast<Dest*>( pAttribute ) = value;
			else
				assert( "Parameter attribute type mismatch!" && false );
		}
	private:
		char* pData = nullptr;
		const VertexLayout& layout;
	};

	class ConstVertex
	{
	public:
		ConstVertex( const Vertex& v ) noexcept(!IS_DEBUG);
		template<VertexLayout::ElementType Type>
		const auto& Attr() const noexcept(!IS_DEBUG)
		{
			return const_cast<Vertex&>( vertex ).Attr<Type>();
		}
	private:
		Vertex vertex;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer( VertexLayout layout, size_t size = 0u ) noexcept(!IS_DEBUG);
		const char* GetData() const noexcept(!IS_DEBUG);
		const VertexLayout& GetLayout() const noexcept;
		void Resize( size_t newSize ) noexcept(!IS_DEBUG);
		size_t Size() const noexcept(!IS_DEBUG);
		size_t SizeBytes() const noexcept(!IS_DEBUG);
		template<typename ...Params>
		void EmplaceBack( Params&&... params ) noexcept(!IS_DEBUG)
		{
			assert( sizeof...( params ) == layout.GetElementCount() && "Param count does not match the number of vertex elements!" );
			buffer.resize( buffer.size() + layout.Size() );
			Back().SetAttributeByIndex( 0u, std::forward<Params>( params )... );
		}
		Vertex Back() noexcept(!IS_DEBUG);
		Vertex Front() noexcept(!IS_DEBUG);
		Vertex operator[]( size_t i ) noexcept(!IS_DEBUG);
		ConstVertex Back() const noexcept(!IS_DEBUG);
		ConstVertex Front() const noexcept(!IS_DEBUG);
		ConstVertex operator[]( size_t i ) const noexcept(!IS_DEBUG);
	private:
		std::vector<char> buffer;
		VertexLayout layout;
	};
}
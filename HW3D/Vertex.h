#pragma once
#include "Graphics.h"
#include <type_traits>
#include <vector>

namespace VertexMeta
{
	struct BGRAColor
	{
		unsigned char a, r, g, b;
	};

	class VertexLayout
	{
	public:
		enum ElementType
		{
			Position2D,
			Position3D,
			Texture2D,
			Normal,
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
		};
		template<> struct Map<Position3D>
		{
			using VertexType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
		};
		template<> struct Map<Texture2D>
		{
			using VertexType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
		};
		template<> struct Map<Normal>
		{
			using VertexType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
		};
		template<> struct Map<Float3Color>
		{
			using VertexType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
		};
		template<> struct Map<Float4Color>
		{
			using VertexType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
		};
		template<> struct Map<BGRAColor>
		{
			using VertexType = VertexMeta::BGRAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
		};

		class Element
		{
		public:
			Element( ElementType type, size_t offset ) : type( type ), offset( offset ) { }
			size_t GetOffsetAfter() const noexcept(!IS_DEBUG)
			{
				return offset + Size();
			}
			size_t GetOffset() const
			{
				return offset;
			}
			size_t Size() const noexcept(!IS_DEBUG)
			{
				return SizeOf( type );
			}
			ElementType GetType() const noexcept
			{
				return type;
			}
			static constexpr size_t SizeOf( ElementType type ) noexcept(!IS_DEBUG)
			{
				switch ( type )
				{
				case Position2D:
					return sizeof( Map<Position2D>::VertexType );
				case Position3D:
					return sizeof( Map<Position3D>::VertexType );
				case Texture2D:
					return sizeof( Map<Texture2D>::VertexType );
				case Normal:
					return sizeof( Map<Normal>::VertexType );
				case Float3Color:
					return sizeof( Map<Float3Color>::VertexType );
				case Float4Color:
					return sizeof( Map<Float4Color>::VertexType );
				case BGRAColor:
					return sizeof( Map<BGRAColor>::VertexType );
				default:
					assert( "Invalid element type!" && false );
				}
				return 0u;
			}
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept(!IS_DEBUG)
			{
				switch ( type )
				{
				case Position2D:
					return GenerateDesc<Position2D>( GetOffset() );
				case Position3D:
					return GenerateDesc<Position3D>( GetOffset() );
				case Texture2D:
					return GenerateDesc<Texture2D>( GetOffset() );
				case Normal:
					return GenerateDesc<Normal>( GetOffset() );
				case Float3Color:
					return GenerateDesc<Float3Color>( GetOffset() );
				case Float4Color:
					return GenerateDesc<Float4Color>( GetOffset() );
				case BGRAColor:
					return GenerateDesc<BGRAColor>( GetOffset() );
				}
				assert( "Invalid element type!" && false );
				return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}
		private:
			template<ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc( size_t offset ) noexcept(!IS_DEBUG)
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
		const Element& ResolveByIndex( size_t i ) const noexcept(!IS_DEBUG)
		{
			return elements[i];
		}
		VertexLayout& Append( ElementType type ) noexcept(!IS_DEBUG)
		{
			elements.emplace_back( type, Size() );
			return *this;
		}
		size_t Size() const noexcept(!IS_DEBUG)
		{
			return elements.empty() ? 0u : elements.back().GetOffsetAfter();
		}
		size_t GetElementCount() const noexcept
		{
			return elements.size();
		}
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept(!IS_DEBUG)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
			desc.reserve( GetElementCount() );
			for ( const auto& e : elements )
			{
				desc.push_back( e.GetDesc() );
			}
			return desc;
		}
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
		Vertex( char* pData, const VertexLayout& layout ) noexcept(!IS_DEBUG) : pData( pData ), layout( layout )
		{
			assert( pData != nullptr );
		}
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
		ConstVertex( const Vertex& v ) noexcept(!IS_DEBUG) : vertex( v ) { }
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
		VertexBuffer( VertexLayout layout ) noexcept(!IS_DEBUG) : layout( std::move( layout ) ) { }
		const char* GetData() const noexcept(!IS_DEBUG)
		{
			return buffer.data();
		}
		const VertexLayout& GetLayout() const noexcept
		{
			return layout;
		}
		size_t Size() const noexcept(!IS_DEBUG)
		{
			return buffer.size() / layout.Size();
		}
		size_t SizeBytes() const noexcept(!IS_DEBUG)
		{
			return buffer.size();
		}
		template<typename ...Params>
		void EmplaceBack( Params&&... params ) noexcept(!IS_DEBUG)
		{
			assert( sizeof...( params ) == layout.GetElementCount() && "Param count does not match the number of vertex elements!" );
			buffer.resize( buffer.size() + layout.Size() );
			Back().SetAttributeByIndex( 0u, std::forward<Params>( params )... );
		}
		Vertex Back() noexcept(!IS_DEBUG)
		{
			assert( buffer.size() != 0u );
			return Vertex{ buffer.data() + buffer.size() - layout.Size(), layout };
		}
		Vertex Front() noexcept(!IS_DEBUG)
		{
			assert( buffer.size() != 0u );
			return Vertex{ buffer.data(), layout };
		}
		Vertex operator[]( size_t i ) noexcept(!IS_DEBUG)
		{
			assert( i < Size() );
			return Vertex{ buffer.data() + layout.Size() * i, layout };
		}
		ConstVertex Back() const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer*>( this )->Back();
		}
		ConstVertex Front() const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer*>( this )->Front();
		}
		ConstVertex operator[]( size_t i  ) const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer&>( *this )[i];
		}
	private:
		std::vector<char> buffer;
		VertexLayout layout;
	};
}
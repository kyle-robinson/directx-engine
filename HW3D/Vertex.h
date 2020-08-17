#pragma once
#include <DirectXMath.h>
#include <type_traits>
#include <vector>

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
		BGRAColor
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
				return sizeof( DirectX::XMFLOAT2 );
			case Position3D:
				return sizeof( DirectX::XMFLOAT3 );
			case Texture2D:
				return sizeof( DirectX::XMFLOAT2 );
			case Normal:
				return sizeof( DirectX::XMFLOAT3 );
			case Float3Color:
				return sizeof( DirectX::XMFLOAT3 );
			case Float4Color:
				return sizeof( DirectX::XMFLOAT3 );
			case BGRAColor:
				return sizeof( ::BGRAColor );
			default:
				assert( "Invalid element type!" && false );
			}
			return 0u;
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
	template<ElementType Type>
	VertexLayout& Append() noexcept(!IS_DEBUG)
	{
		elements.emplace_back( Type, Size() );
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
		const auto& element = layout.Resolve<Type>();
		auto pAttribute = pData + element.GetOffset();
		if constexpr( Type == VertexLayout::Position2D )
		{
			return *reinterpret_cast<DirectX::XMFLOAT2*>( pAttribute );
		}
		else if constexpr ( Type == VertexLayout::Position3D )
		{
			return *reinterpret_cast<DirectX::XMFLOAT3*>( pAttribute );
		}
		else if constexpr ( Type == VertexLayout::Texture2D )
		{
			return *reinterpret_cast<DirectX::XMFLOAT2*>( pAttribute );
		}
		else if constexpr ( Type == VertexLayout::Normal )
		{
			return *reinterpret_cast<DirectX::XMFLOAT3*>( pAttribute );
		}
		else if constexpr ( Type == VertexLayout::Float3Color )
		{
			return *reinterpret_cast<DirectX::XMFLOAT3*>( pAttribute );
		}
		else if constexpr ( Type == VertexLayout::Float4Color )
		{
			return *reinterpret_cast<DirectX::XMFLOAT4*>( pAttribute );
		}
		else if constexpr ( Type == VertexLayout::BGRAColor )
		{
			return *reinterpret_cast<BGRAColor*>( pAttribute );
		}
		else
		{
			assert( "Bad element type!" && false );
			return *reinterpret_cast<char*>( pAttribute );
		}
	}
	template<typename T>
	void SetAttributeByIndex( size_t i, T&& value ) noexcept(!IS_DEBUG)
	{
		const auto& element = layout.ResolveByIndex( i );
		auto pAttribute = pData + element.GetOffset();
		switch ( element.GetType() )
		{
		case VertexLayout::Position2D:
			SetAttribute<DirectX::XMFLOAT2>( pAttribute, std::forward<T>( value ) );
			break;
		case VertexLayout::Position3D:
			SetAttribute<DirectX::XMFLOAT3>( pAttribute, std::forward<T>( value ) );
			break;
		case VertexLayout::Texture2D:
			SetAttribute<DirectX::XMFLOAT2>( pAttribute, std::forward<T>( value ) );
			break;
		case VertexLayout::Normal:
			SetAttribute<DirectX::XMFLOAT3>( pAttribute, std::forward<T>( value ) );
			break;
		case VertexLayout::Float3Color:
			SetAttribute<DirectX::XMFLOAT3>( pAttribute, std::forward<T>( value ) );
			break;
		case VertexLayout::Float4Color:
			SetAttribute<DirectX::XMFLOAT4>( pAttribute, std::forward<T>( value ) );
			break;
		case VertexLayout::BGRAColor:
			SetAttribute<BGRAColor>( pAttribute, std::forward<T>( value ) );
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
	template<typename Dest, typename Src>
	void SetAttribute( char* pAttribute, Src&& value ) noexcept(!IS_DEBUG)
	{
		if constexpr( std::is_assignable<Dest, Src>::value )
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
	const VertexLayout& GetLayout() const noexcept
	{
		return layout;
	}
	size_t Size() const noexcept(!IS_DEBUG)
	{
		return buffer.size() / layout.Size();
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
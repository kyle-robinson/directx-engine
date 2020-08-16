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
				return sizeof( unsigned int );
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
private:
	std::vector<Element> elements;
};

class Vertex
{
	friend class VertexBuffer;
public:

};

class VertexBuffer
{
public:

};
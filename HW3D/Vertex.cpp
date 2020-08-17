#include "Vertex.h"

namespace VertexMeta
{
	// VertexLayout
	const VertexLayout::Element& VertexLayout::ResolveByIndex(size_t i) const noexcept(!IS_DEBUG)
	{
		return elements[i];
	}

	VertexLayout& VertexLayout::Append(ElementType type) noexcept(!IS_DEBUG)
	{
		elements.emplace_back(type, Size());
		return *this;
	}

	size_t VertexLayout::Size() const noexcept(!IS_DEBUG)
	{
		return elements.empty() ? 0u : elements.back().GetOffsetAfter();
	}

	size_t VertexLayout::GetElementCount() const noexcept
	{
		return elements.size();
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout() const noexcept(!IS_DEBUG)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve(GetElementCount());
		for (const auto& e : elements)
		{
			desc.push_back(e.GetDesc());
		}
		return desc;
	}

	// VertexLayout::Element
	VertexLayout::Element::Element( ElementType type, size_t offset ) : type( type ), offset( offset ) { }

	size_t VertexLayout::Element::GetOffsetAfter() const noexcept(!IS_DEBUG)
	{
		return offset + Size();
	}

	size_t VertexLayout::Element::GetOffset() const
	{
		return offset;
	}

	size_t VertexLayout::Element::Size() const noexcept(!IS_DEBUG)
	{
		return SizeOf(type);
	}

	VertexLayout::ElementType VertexLayout::Element::GetType() const noexcept
	{
		return type;
	}

	constexpr size_t VertexLayout::Element::SizeOf(ElementType type) noexcept(!IS_DEBUG)
	{
		switch (type)
		{
		case Position2D:
			return sizeof(Map<Position2D>::VertexType);
		case Position3D:
			return sizeof(Map<Position3D>::VertexType);
		case Texture2D:
			return sizeof(Map<Texture2D>::VertexType);
		case Normal:
			return sizeof(Map<Normal>::VertexType);
		case Float3Color:
			return sizeof(Map<Float3Color>::VertexType);
		case Float4Color:
			return sizeof(Map<Float4Color>::VertexType);
		case BGRAColor:
			return sizeof(Map<BGRAColor>::VertexType);
		default:
			assert("Invalid element type!" && false);
		}
		return 0u;
	}

	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noexcept(!IS_DEBUG)
	{
		switch (type)
		{
		case Position2D:
			return GenerateDesc<Position2D>(GetOffset());
		case Position3D:
			return GenerateDesc<Position3D>(GetOffset());
		case Texture2D:
			return GenerateDesc<Texture2D>(GetOffset());
		case Normal:
			return GenerateDesc<Normal>(GetOffset());
		case Float3Color:
			return GenerateDesc<Float3Color>(GetOffset());
		case Float4Color:
			return GenerateDesc<Float4Color>(GetOffset());
		case BGRAColor:
			return GenerateDesc<BGRAColor>(GetOffset());
		}
		assert("Invalid element type!" && false);
		return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	// Vertex
	Vertex::Vertex( char* pData, const VertexLayout& layout ) noexcept(!IS_DEBUG) : pData(pData), layout(layout)
	{
		assert(pData != nullptr);
	}

	ConstVertex::ConstVertex( const Vertex& v ) noexcept(!IS_DEBUG) : vertex(v) { }

	// VertexBuffer
	VertexBuffer::VertexBuffer( VertexLayout layout ) noexcept(!IS_DEBUG) : layout(std::move(layout)) { }

	const char* VertexBuffer::GetData() const noexcept(!IS_DEBUG)
	{
		return buffer.data();
	}

	const VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return layout;
	}

	size_t VertexBuffer::Size() const noexcept(!IS_DEBUG)
	{
		return buffer.size() / layout.Size();
	}

	size_t VertexBuffer::SizeBytes() const noexcept(!IS_DEBUG)
	{
		return buffer.size();
	}

	Vertex VertexBuffer::Back() noexcept(!IS_DEBUG)
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data() + buffer.size() - layout.Size(), layout };
	}

	Vertex VertexBuffer::Front() noexcept(!IS_DEBUG)
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data(), layout };
	}

	Vertex VertexBuffer::operator[](size_t i) noexcept(!IS_DEBUG)
	{
		assert(i < Size());
		return Vertex{ buffer.data() + layout.Size() * i, layout };
	}

	ConstVertex VertexBuffer::Back() const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer*>(this)->Back();
	}

	ConstVertex VertexBuffer::Front() const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}

	ConstVertex VertexBuffer::operator[](size_t i) const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}
}
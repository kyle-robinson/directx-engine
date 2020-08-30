#pragma once
#include "Graphics.h"
#include "Technique.h"
#include <memory>

class TechniqueProbe;

namespace Bind
{
	class IndexBuffer;
	class InputLayout;
	class Topology;
	class VertexBuffer;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable( const Drawable& ) = delete;
	void AddTechnique( Technique tech_in ) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit( class FrameCommander& frame ) const noexcept;
	void Bind( Graphics& gfx ) const noexcept;
	void Accept( TechniqueProbe& );
	UINT GetIndexCount() const noexcept(!IS_DEBUG);
	virtual ~Drawable();
protected:
	std::vector<Technique> techniques;
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
};
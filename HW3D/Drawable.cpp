#include "Drawable.h"
#include "BindableCodex.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"

using namespace Bind;

void Drawable::Submit( FrameCommander& frame ) const noexcept
{
	for ( const auto& tech : techniques )
		tech.Submit( frame, *this );
}

void Drawable::AddTechnique( Technique tech_in ) noexcept
{
	tech_in.InitializeParentReference( *this );
	techniques.push_back( std::move( tech_in ) );
}

void Drawable::Bind( Graphics& gfx ) const noexcept
{
	pIndices->Bind( gfx );
	pTopology->Bind( gfx );
	pVertices->Bind( gfx );
}

void Drawable::Accept( TechniqueProbe& probe )
{
	for ( auto t : techniques )
	{
		t.Accept( probe );
	}
}

UINT Drawable::GetIndexCount() const noexcept(!IS_DEBUG)
{
	return pIndices->GetCount();
}

Drawable::~Drawable() {}
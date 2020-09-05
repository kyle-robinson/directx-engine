#include "Drawable.h"
#include "Material.h"
#include "BindableCodex.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include <assimp/scene.h>

using namespace Bind;

Drawable::Drawable( Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale ) noexcept
{
	pVertices = mat.MakeVertexBindable( gfx, mesh, scale );
	pIndices = mat.MakeIndexBindable( gfx, mesh );
	pTopology = Bind::Topology::Resolve( gfx );

	for ( auto& t : mat.GetTechniques() )
		AddTechnique( std::move( t ) );
}

void Drawable::Submit() const noexcept
{
	for ( const auto& tech : techniques )
		tech.Submit( *this );
}

void Drawable::AddTechnique( Technique tech_in ) noexcept
{
	tech_in.InitializeParentReference( *this );
	techniques.push_back( std::move( tech_in ) );
}

void Drawable::Bind( Graphics& gfx ) const noexcept(!IS_DEBUG)
{
	pIndices->Bind( gfx );
	pTopology->Bind( gfx );
	pVertices->Bind( gfx );
}

void Drawable::Accept( TechniqueProbe& probe )
{
	for ( auto& t : techniques )
		t.Accept( probe );
}

UINT Drawable::GetIndexCount() const noexcept(!IS_DEBUG)
{
	return pIndices->GetCount();
}

void Drawable::LinkTechniques( RenderGraph& rg )
{
	for ( auto& tech : techniques )
	{
		tech.Link( rg );
	}
}

Drawable::~Drawable() {}
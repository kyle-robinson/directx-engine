#pragma once
#include "BindableCommon.h"
#include "Technique.h"
#include <assimp/scene.h>
#include <filesystem>
#include <vector>

class Material
{
public:
	Material( Graphics& gfx, const aiMaterial* pMaterial, const std::filesystem::path& path ) noexcept(!IS_DEBUG) { }
	VertexMeta::VertexBuffer ExtractVertices( const aiMesh& mesh ) const noexcept
	{
		VertexMeta::VertexBuffer buf{ layout };
		buf.Resize( mesh.mNumVertices );
		if ( layout.Has<VertexMeta::VertexLayout::ElementType::Position3D>() )
		{
			for ( int i = 0; i < mesh.mNumVertices; i++ )
				buf[i];
		}
	}
	std::vector<Technique> GetTechniques() const noexcept
	{
		return techniques;
	}
private:
	VertexMeta::VertexLayout layout;
	std::vector<Technique> techniques;
};
#pragma once
#include "BindableCommon.h"
#include "Technique.h"
#include <assimp/scene.h>
#include <filesystem>
#include <vector>
#include "DynamicConstant.h"
#include "ConstantBufferEx.h"

class Material
{
public:
	Material( Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path ) noexcept(!IS_DEBUG);
	VertexMeta::VertexBuffer ExtractVertices( const aiMesh& mesh ) const noexcept;
	std::vector<unsigned short> ExtractIndices( const aiMesh& mesh ) const noexcept
	{
		std::vector<unsigned short> indices;
		indices.reserve( mesh.mNumFaces * 3 );
		for ( unsigned int i = 0; i < mesh.mNumFaces; i++ )
		{
			const auto& face = mesh.mFaces[i];
			assert( face.mNumIndices == 3 );
			indices.push_back( face.mIndices[0] );
			indices.push_back( face.mIndices[1] );
			indices.push_back( face.mIndices[2] );
		}
		return indices;
	}
	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable( Graphics& gfx, const aiMesh& mesh ) const noexcept(!IS_DEBUG)
	{
		return Bind::VertexBuffer::Resolve( gfx, MakeMeshTag( mesh ), ExtractVertices( mesh ) );
	}
	std::shared_ptr<Bind::IndexBuffer> MakeIndexBindable( Graphics& gfx, const aiMesh& mesh ) const noexcept(!IS_DEBUG)
	{
		return Bind::IndexBuffer::Resolve( gfx, MakeMeshTag( mesh ), ExtractIndices( mesh ) );
	}
	std::vector<Technique> GetTechniques() const noexcept;
private:
	std::string MakeMeshTag( const aiMesh& mesh ) const noexcept
	{
		return modelPath + "%" + mesh.mName.C_Str();
	}
private:
	VertexMeta::VertexLayout layout;
	std::vector<Technique> techniques;
	std::string modelPath;
	std::string name;
};
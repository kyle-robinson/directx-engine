#include "AssimpObject.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

AssimpObject::AssimpObject( Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	DirectX::XMFLOAT3 material, float scale )
	:
	PrimitiveObject( gfx, rng, adist, ddist, odist, rdist )
{
	if ( !IsStaticInitialised() )
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};

		// setup importer and load model scene
		Assimp::Importer asmpImporter;
		const auto pModel = asmpImporter.ReadFile(
			"res\\models\\backpack\\backpack.obj",
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
		);

		if ( !pModel || pModel->mFlags == AI_SCENE_FLAGS_INCOMPLETE || pModel->mRootNode )
			assert( "ERROR::ASSIMP:: " && asmpImporter.GetErrorString() );
		
		const auto pMesh = pModel->mMeshes[0];

		// load model vertices
		std::vector<Vertex> vertices;
		vertices.reserve( pMesh->mNumVertices );
		for ( unsigned int i = 0; i < pMesh->mNumVertices; i++ )
		{
			vertices.push_back(
				{ { pMesh->mVertices[i].x * scale, pMesh->mVertices[i].y * scale, pMesh->mVertices[i].z * scale },
				*reinterpret_cast<DirectX::XMFLOAT3*>( &pMesh->mNormals[i] ) }
			);
		}

		// load model indices
		std::vector<unsigned short> indices;
		indices.reserve( pMesh->mNumFaces );
		for ( unsigned int i = 0; i < pMesh->mNumFaces; i++ )
		{
			const auto& face = pMesh->mFaces[i];
			assert( face.mNumIndices == 3 );
			indices.push_back( face.mIndices[0] );
			indices.push_back( face.mIndices[1] );
			indices.push_back( face.mIndices[2] );
		}

		// Add static bindables
		AddStaticBind( std::make_unique<VertexBuffer>( gfx, vertices ) );
		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx, indices ) );

		auto pvs = std::make_unique<VertexShader>( gfx, L"PhongVS.cso" );
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind( std::move( pvs ) );
		AddStaticBind( std::make_unique<PixelShader>( gfx, L"PhongPS.cso" ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx, ied, pvsbc ) );
		AddStaticBind( std::make_unique<Topology>( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color;
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[3];
		} pmc;
		pmc.color = material;
		AddStaticBind( std::make_unique<PixelConstantBuffer<PSMaterialConstant>>( gfx, pmc, 1u ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<TransformCbuf>( gfx, *this ) );
}
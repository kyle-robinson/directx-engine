#include "Model.h"
#include "ModelWindow.h"
#include "ModelException.h"
#include "Node.h"
#include "Mesh.h"
#include "Material.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

Model::Model(Graphics& gfx, const std::string& pathString, const float scale)// : pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer importer;
	const auto pScene = importer.ReadFile(
		pathString.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (pScene == nullptr)
		throw ModelException(__LINE__, __FILE__, importer.GetErrorString());

	std::vector<Material> materials;
	materials.reserve( pScene->mNumMaterials );
	for ( size_t i = 0; i < pScene->mNumMaterials; i++ )
		materials.emplace_back( gfx, *pScene->mMaterials[i], pathString );

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		const auto& mesh = *pScene->mMeshes[i];
		meshPtrs.push_back( std::make_unique<Mesh>( gfx, materials[mesh.mMaterialIndex], mesh ) );
	}

	int nextID = 0;
	pRoot = ParseNode( nextID, *pScene->mRootNode, DirectX::XMMatrixScaling( scale, scale, scale ) );
}

void Model::Submit(FrameCommander& frame) const noexcept(!IS_DEBUG)
{
	//pWindow->ApplyParameters();
	pRoot->Submit(frame, DirectX::XMMatrixIdentity());
}

/*void Model::ShowControlWindow(Graphics& gfx, const char* windowName) noexcept
{
	pWindow->Show(gfx, windowName, *pRoot);
}*/

void Model::SetRootTransform(DirectX::FXMMATRIX tf) noexcept
{
	pRoot->SetAppliedTransform(tf);
}

/*std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale)
{
	return {};
}*/

std::unique_ptr<Node> Model::ParseNode( int& nextID, const aiNode& node, DirectX::FXMMATRIX additionalTransform ) noexcept
{
	const auto transform = additionalTransform * DirectX::XMMatrixTranspose(
		DirectX::XMLoadFloat4x4(
			reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
		)
	);

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(nextID++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
		pNode->AddChild( ParseNode( nextID, *node.mChildren[i], DirectX::XMMatrixIdentity() ) );

	return pNode;
}

Model::~Model() noexcept { }
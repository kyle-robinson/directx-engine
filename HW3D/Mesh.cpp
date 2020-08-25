#include "Mesh.h"
#include "Surface.h"
#include "MathX.h"
#include <unordered_map>
#include <sstream>
#include <iostream>

// ModelException
ModelException::ModelException( int line, const char* file, std::string note ) noexcept : Exception( line, file ), note( note ) { }

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << Exception::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}

// Mesh
Mesh::Mesh( Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs )
{
	AddBind( Bind::Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	for (auto& pb : bindPtrs)
	{
		AddBind( std::move( pb ) );
	}

	AddBind( std::make_shared<Bind::TransformCbuf>( gfx, *this ) );
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}
DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}

// Node
Node::Node( int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform ) noexcept(!IS_DEBUG)
	: meshPtrs( std::move( meshPtrs ) ), name( name ), id( id )
{
	DirectX::XMStoreFloat4x4( &baseTransform, transform );
	DirectX::XMStoreFloat4x4( &appliedTransform, DirectX::XMMatrixIdentity() );
}

void Node::Draw( Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform ) const noexcept(!IS_DEBUG)
{
	const auto built =
		DirectX::XMLoadFloat4x4( &baseTransform ) *
		DirectX::XMLoadFloat4x4( &appliedTransform ) *
		accumulatedTransform;
	for (const auto pm : meshPtrs)
	{
		pm->Draw(gfx, built);
	}
	for (const auto& pc : childPtrs)
	{
		pc->Draw(gfx, built);
	}
}

void Node::RenderTree( Node*& pSelectedNode ) const noexcept
{
	// set id to impossible value, if no node is selected
	const int selectedID = ( pSelectedNode == nullptr ) ? -1 : pSelectedNode->GetID();

	// build flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| (( GetID() == selectedID ) ? ImGuiTreeNodeFlags_Selected : 0 )
		| (( childPtrs.empty() ) ? ImGuiTreeNodeFlags_Leaf : 0 );

	// render current node
	const auto expanded = ImGui::TreeNodeEx(
		(void*)(intptr_t)GetID(), node_flags, name.c_str()
	);

	// for selecting nodes
	if ( ImGui::IsItemClicked() )
	{
		pSelectedNode = const_cast<Node*>(this);
	}

	// if node is expanded, recursively render all children
	if ( expanded )
	{
		for ( const auto& pChild : childPtrs )
		{
			pChild->RenderTree( pSelectedNode );
		}
		ImGui::TreePop();
	}
}

void Node::SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept
{
	DirectX::XMStoreFloat4x4( &appliedTransform, transform );
}

const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept
{
	return appliedTransform;
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

int Node::GetID() const noexcept
{
	return id;
}

// Model
class ModelWindow
{
public:
	void Show( Graphics& gfx, const char* windowName, const Node& root ) noexcept
	{
		windowName = windowName ? windowName : "Model";
		int nodeIndexTracker = 0;

		if ( ImGui::Begin( windowName ) )
		{
			ImGui::Columns( 2, nullptr, true );
			root.RenderTree( pSelectedNode );

			ImGui::NextColumn();
			if ( pSelectedNode != nullptr )
			{
				const auto id = pSelectedNode->GetID();
				auto i = transforms.find( id );
				if ( i == transforms.end() )
				{
					const auto& applied = pSelectedNode->GetAppliedTransform();
					const auto angles = ExtractEulerAngles( applied );
					const auto translation = ExtractTranslation( applied );

					TransformParameters tp;
					tp.roll = angles.z;
					tp.pitch = angles.x;
					tp.yaw = angles.y;
					tp.x = translation.x;
					tp.y = translation.y;
					tp.z = translation.z;
					std::tie( i, std::ignore ) = transforms.insert( { id, tp } );
				}
				auto& transform = i->second;

				if ( ImGui::CollapsingHeader( "Orientation" ) )
				{
					ImGui::SliderAngle( "Roll", &transform.roll, -180.0f, 180.0f );
					ImGui::SliderAngle( "Pitch", &transform.pitch, -180.0f, 180.0f );
					ImGui::SliderAngle( "Yaw", &transform.yaw, -180.0f, 180.0f );
				}

				if ( ImGui::CollapsingHeader( "Position" ) )
				{
					ImGui::SliderFloat( "X", &transform.x, -20.0f, 20.0f );
					ImGui::SliderFloat( "Y", &transform.y, -20.0f, 20.0f );
					ImGui::SliderFloat( "Z", &transform.z, -20.0f, 20.0f );
				}

				if ( ImGui::CollapsingHeader( "Material" ) )
					if ( !pSelectedNode->ControlWindow( gfx, skinMaterial ) )
						if( !pSelectedNode->ControlWindow( gfx, eyeMaterial ) )
							pSelectedNode->ControlWindow( gfx, ringMaterial );

				if ( ImGui::Button( "Reset" ) )
					ResetMesh();
			}
		}
		ImGui::End();
	}
	void ResetMesh() noexcept
	{
		auto& transform = transforms.at( pSelectedNode->GetID() );
		transform.roll = 0.0f;
		transform.pitch = 0.0f;
		transform.yaw = 0.0f;
		transform.x = 0.0f;
		transform.y = 0.0f;
		transform.z = 0.0f;
	}
	DirectX::XMMATRIX GetTransform() const noexcept
	{
		assert( pSelectedNode != nullptr );
		const auto& transform = transforms.at( pSelectedNode->GetID() );

		return
			DirectX::XMMatrixRotationRollPitchYaw( transform.roll, transform.pitch, transform.yaw ) *
			DirectX::XMMatrixTranslation( transform.x, transform.y, transform.z );
	}
	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}
private:
	Node* pSelectedNode;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	Node::PSMaterialConstantFull skinMaterial;
	Node::PSMaterialConstantDiffuse eyeMaterial;
	Node::PSMaterialConstantNoTexture ringMaterial;
	std::unordered_map<int, TransformParameters> transforms;
};

Model::Model( Graphics& gfx, const std::string& pathString, float scale ) : pWindow( std::make_unique<ModelWindow>() )
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

	if ( pScene == nullptr )
		throw ModelException( __LINE__, __FILE__, importer.GetErrorString() );

	for ( size_t i = 0; i < pScene->mNumMeshes; i++ )
		meshPtrs.push_back( ParseMesh( gfx, *pScene->mMeshes[i], pScene->mMaterials, pathString, scale ) );

	int nextID = 0;
	pRoot = ParseNode( nextID, *pScene->mRootNode );
}

void Model::Draw( Graphics& gfx ) const noexcept(!IS_DEBUG)
{
	if ( auto node = pWindow->GetSelectedNode() )
	{
		node->SetAppliedTransform( pWindow->GetTransform() );
	}
	pRoot->Draw( gfx, DirectX::XMMatrixIdentity() );
}

void Model::ShowControlWindow( Graphics& gfx, const char* windowName ) noexcept
{
	pWindow->Show( gfx, windowName, *pRoot );
}

void Model::SetRootTransform( DirectX::FXMMATRIX tf ) noexcept
{
	pRoot->SetAppliedTransform( tf );
}

std::unique_ptr<Mesh> Model::ParseMesh( Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale )
{
	using VertexMeta::VertexLayout;
	using namespace std::string_literals;

	const auto rootPath = path.parent_path().string() + "\\";
	std::vector<std::shared_ptr<Bind::Bindable>> bindablePtrs;

	bool hasDiffuseMap = false;
	bool hasSpecularMap = false;
	bool hasAlphaDiffuse = false;
	bool hasNormalMap = false;
	bool hasAlphaGloss = false;
	float shininess = 2.0f;
	DirectX::XMFLOAT4 diffuseColor = { 0.45f, 0.45f, 0.85f, 1.0f };
	DirectX::XMFLOAT4 specularColor = { 0.18f, 0.18f, 0.18f, 1.0f };

	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString texFileName;

		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
		{
			auto tex = std::make_unique<Bind::Texture>( gfx, rootPath + texFileName.C_Str() );
			hasAlphaDiffuse = tex->HasAlpha();
			bindablePtrs.push_back( std::move( tex ) );
			hasDiffuseMap = true;
		}
		else
		{
			material.Get( AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>( diffuseColor ) );
		}

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			auto tex = std::make_unique<Bind::Texture>( gfx, rootPath + texFileName.C_Str(), 1 );
			hasAlphaGloss = tex->HasAlpha();
			bindablePtrs.push_back( std::move( tex ) );
			hasSpecularMap = true;
		}
		else
		{
			material.Get( AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>( specularColor ) );
		}
		if ( !hasAlphaGloss )
		{
			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
		{
			bindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, rootPath + texFileName.C_Str(), 2));
			hasNormalMap = true;
		}

		if ( hasDiffuseMap || hasSpecularMap || hasNormalMap )
			bindablePtrs.push_back(Bind::Sampler::Resolve(gfx));
	}

	const auto meshTag = path.string() + "%" + mesh.mName.C_Str();

	if ( hasDiffuseMap && hasSpecularMap && hasNormalMap )
	{
		VertexMeta::VertexBuffer vbuf(
			std::move(
				VertexLayout{}
				.Append( VertexLayout::Position3D )
				.Append( VertexLayout::Normal )
				.Append( VertexLayout::Tangent )
				.Append( VertexLayout::Bitangent )
				.Append( VertexLayout::Texture2D )
			)
		);

		for ( unsigned int i = 0; i < mesh.mNumVertices; i++ )
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3( mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale ),
				*reinterpret_cast<DirectX::XMFLOAT3*>( &mesh.mNormals[i] ),
				*reinterpret_cast<DirectX::XMFLOAT3*>( &mesh.mTangents[i] ),
				*reinterpret_cast<DirectX::XMFLOAT3*>( &mesh.mBitangents[i] ),
				*reinterpret_cast<DirectX::XMFLOAT2*>( &mesh.mTextureCoords[0][i] )
			);
		}

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

		bindablePtrs.push_back( Bind::VertexBuffer::Resolve( gfx, meshTag, vbuf ) );
		bindablePtrs.push_back( Bind::IndexBuffer::Resolve( gfx, meshTag, indices ) );

		auto pvs = Bind::VertexShader::Resolve( gfx, "PhongVSNormal.cso" );
		auto pvsbc = pvs->GetByteCode();
		bindablePtrs.push_back( std::move( pvs ) );

		bindablePtrs.push_back( Bind::PixelShader::Resolve( gfx, hasAlphaDiffuse ? "PhongPSSpecNormalMask.cso" : "PhongPSSpecNormal.cso" ) );
		
		bindablePtrs.push_back( Bind::InputLayout::Resolve( gfx, vbuf.GetLayout(), pvsbc ) );

		Node::PSMaterialConstantFull pmc;
		pmc.specularPower = shininess;
		pmc.hasGlossMap = hasAlphaGloss ? TRUE : FALSE;
		bindablePtrs.push_back( Bind::PixelConstantBuffer<Node::PSMaterialConstantFull>::Resolve( gfx, pmc, 1u ) );
	}
	else if ( hasDiffuseMap && hasNormalMap )
	{
		VertexMeta::VertexBuffer vbuf(
			std::move(
				VertexLayout{}
				.Append(VertexLayout::Position3D)
				.Append(VertexLayout::Normal)
				.Append(VertexLayout::Tangent)
				.Append(VertexLayout::Bitangent)
				.Append(VertexLayout::Texture2D)
			)
		);

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x* scale, mesh.mVertices[i].y* scale, mesh.mVertices[i].z* scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(Bind::VertexBuffer::Resolve(gfx, meshTag, vbuf));
		bindablePtrs.push_back(Bind::IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = Bind::VertexShader::Resolve(gfx, "PhongVSNormal.cso");
		auto pvsbc = pvs->GetByteCode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(Bind::PixelShader::Resolve(gfx, "PhongPSNormal.cso"));

		bindablePtrs.push_back(Bind::InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstant
		{
			float specularIntensity;
			float specularPower;
			BOOL normalMapEnabled = TRUE;
			float padding;
		} pmc;
		pmc.specularPower = shininess;
		pmc.specularIntensity = ( specularColor.x + specularColor.y + specularColor.z ) / 3.0f;
		bindablePtrs.push_back(Bind::PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
	}
	else if ( hasDiffuseMap && !hasNormalMap && hasSpecularMap )
	{
		VertexMeta::VertexBuffer vbuf(
			std::move(
				VertexLayout{}
				.Append(VertexLayout::Position3D)
				.Append(VertexLayout::Normal)
				.Append(VertexLayout::Texture2D)
			)
		);

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(Bind::VertexBuffer::Resolve(gfx, meshTag, vbuf));
		bindablePtrs.push_back(Bind::IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = Bind::VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(Bind::PixelShader::Resolve(gfx, "PhongPSSpec.cso"));

		bindablePtrs.push_back(Bind::InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstant
		{
			float specularPowerConst;
			BOOL hasGloss;
			float specularMapWeight;
			float padding;
		} pmc;
		pmc.specularPowerConst = shininess;
		pmc.hasGloss = hasAlphaGloss ? TRUE : FALSE;
		pmc.specularMapWeight = 1.0f;
		bindablePtrs.push_back(Bind::PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
	}
	else if ( hasDiffuseMap )
	{
		VertexMeta::VertexBuffer vbuf(
			std::move(
				VertexLayout{}
				.Append(VertexLayout::Position3D)
				.Append(VertexLayout::Normal)
				.Append(VertexLayout::Texture2D)
			)
		);

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(Bind::VertexBuffer::Resolve(gfx, meshTag, vbuf));
		bindablePtrs.push_back(Bind::IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = Bind::VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(Bind::PixelShader::Resolve(gfx, "PhongPS.cso"));

		bindablePtrs.push_back(Bind::InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		Node::PSMaterialConstantDiffuse pmc;
		pmc.specularPower = shininess;
		pmc.specularIntensity = ( specularColor.x + specularColor.y + specularColor.z ) / 3.0f;
		bindablePtrs.push_back(Bind::PixelConstantBuffer<Node::PSMaterialConstantDiffuse>::Resolve(gfx, pmc, 1u));
	}
	else if ( !hasDiffuseMap && !hasSpecularMap && !hasNormalMap )
	{
		VertexMeta::VertexBuffer vbuf(
			std::move(
				VertexLayout{}
				.Append(VertexLayout::Position3D)
				.Append(VertexLayout::Normal)
			)
		);

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(Bind::VertexBuffer::Resolve(gfx, meshTag, vbuf));
		bindablePtrs.push_back(Bind::IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = Bind::VertexShader::Resolve(gfx, "PhongVSNoTexture.cso");
		auto pvsbc = pvs->GetByteCode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(Bind::PixelShader::Resolve(gfx, "PhongPSNoTexture.cso"));

		bindablePtrs.push_back(Bind::InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		Node::PSMaterialConstantNoTexture pmc;
		pmc.specularPower = shininess;
		pmc.materialColor = diffuseColor;
		pmc.specularColor = specularColor;
		bindablePtrs.push_back( Bind::PixelConstantBuffer<Node::PSMaterialConstantNoTexture>::Resolve( gfx, pmc, 1u ) );
	}
	else
	{
		throw std::runtime_error( "ERROR::MATERIAL:: Was not able to load any materials!" );
	}

	bindablePtrs.push_back( std::make_unique<Bind::Rasterizer>( gfx, hasAlphaDiffuse ) );

	return std::make_unique<Mesh>( gfx, std::move( bindablePtrs ) );
}

std::unique_ptr<Node> Model::ParseNode( int& nextID, const aiNode& node ) noexcept
{
	const auto transform = DirectX::XMMatrixTranspose(
		DirectX::XMLoadFloat4x4(
			reinterpret_cast<const DirectX::XMFLOAT4X4*>( &node.mTransformation )
		)
	);

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve( node.mNumMeshes );
	for ( size_t i = 0; i < node.mNumMeshes; i++ )
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back( meshPtrs.at( meshIdx ).get() );
	}

	auto pNode = std::make_unique<Node>( nextID++, node.mName.C_Str(), std::move( curMeshPtrs ), transform );
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild( ParseNode( nextID, *node.mChildren[i] ) );
	}

	return pNode;
}

Model::~Model() noexcept { }
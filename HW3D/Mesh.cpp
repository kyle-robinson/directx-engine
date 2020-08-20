#include "Mesh.h"
#include "imgui/imgui.h"
#include <unordered_map>
#include <sstream>

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
Mesh::Mesh( Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs )
{
	if (!IsStaticInitialised())
	{
		AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	for (auto& pb : bindPtrs)
	{
		if ( auto pi = dynamic_cast<Bind::IndexBuffer*>( pb.get() ) )
		{
			AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer>{ pi });
			pb.release();
		}
		else
		{
			AddBind( std::move( pb ) );
		}
	}

	AddBind( std::make_unique<Bind::TransformCbuf>( gfx, *this ) );
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

void Node::RenderTree( std::optional<int>& selectedIndex, Node*& pSelectedNode ) const noexcept
{
	// build flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| (( GetID() == selectedIndex.value_or( -1 ) ) ? ImGuiTreeNodeFlags_Selected : 0 )
		| (( childPtrs.empty() ) ? ImGuiTreeNodeFlags_Leaf : 0 );

	// render current node
	const auto expanded = ImGui::TreeNodeEx(
		(void*)(intptr_t)GetID(), node_flags, name.c_str()
	);

	// for selecting nodes
	if ( ImGui::IsItemClicked() )
	{
		selectedIndex = GetID();
		pSelectedNode = const_cast<Node*>(this);
	}

	// if node is expanded, recursively render all children
	if ( expanded )
	{
		for ( const auto& pChild : childPtrs )
		{
			pChild->RenderTree( selectedIndex, pSelectedNode );
		}
		ImGui::TreePop();
	}
}

void Node::SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept
{
	DirectX::XMStoreFloat4x4( &appliedTransform, transform );
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
	void Show( const char* windowName, const Node& root ) noexcept
	{
		windowName = windowName ? windowName : "Model";
		int nodeIndexTracker = 0;

		if ( ImGui::Begin( windowName ) )
		{
			ImGui::Columns( 2, nullptr, true );
			root.RenderTree( selectedIndex, pSelectedNode );

			ImGui::NextColumn();
			if ( pSelectedNode != nullptr )
			{
				auto& transform = transforms[*selectedIndex];

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

				if ( ImGui::Button( "Reset" ) )
					ResetMesh();
			}
		}
		ImGui::End();
	}
	void ResetMesh() noexcept
	{
		auto& transform = transforms.at( *selectedIndex );
		transform.roll = 0.0f;
		transform.pitch = 0.0f;
		transform.yaw = 0.0f;
		transform.x = 0.0f;
		transform.y = 0.0f;
		transform.z = 0.0f;
	}
	DirectX::XMMATRIX GetTransform() noexcept
	{
		const auto& transform = transforms.at( *selectedIndex );
		
		if ( !meshInitialized )
		{
			meshInitialized = true;
			ResetMesh();
		}

		return
			DirectX::XMMatrixRotationRollPitchYaw( transform.roll, transform.pitch, transform.yaw ) *
			DirectX::XMMatrixTranslation( transform.x, transform.y, transform.z );
	}
	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}
private:
	std::optional<int> selectedIndex;
	Node* pSelectedNode;
	struct TransformParameters
	{
		float roll, pitch, yaw, x, y, z;
	};
	bool meshInitialized = false;
	std::unordered_map<int, TransformParameters> transforms;
};

Model::Model( Graphics& gfx, const std::string fileName ) : pWindow( std::make_unique<ModelWindow>() )
{
	Assimp::Importer importer;
	const auto pScene = importer.ReadFile(
		fileName.c_str(),
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
	);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
	}

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

void Model::ShowControlWindow( const char* windowName ) noexcept
{
	pWindow->Show( windowName, *pRoot );
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh)
{
	using VertexMeta::VertexLayout;

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
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
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

	std::vector<std::unique_ptr<Bind::Bindable>> bindablePtrs;
	bindablePtrs.push_back(std::make_unique<Bind::VertexBuffer>(gfx, vbuf));
	bindablePtrs.push_back(std::make_unique<Bind::IndexBuffer>(gfx, indices));

	auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"PhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	bindablePtrs.push_back(std::move(pvs));
	bindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"PhongPS.cso"));

	bindablePtrs.push_back(std::make_unique<Bind::InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

	struct PSMaterialCount
	{
		DirectX::XMFLOAT3 color = { 0.6f, 0.6f, 0.8f };
		float speuclarIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} pmc;
	bindablePtrs.push_back(std::make_unique<Bind::PixelConstantBuffer<PSMaterialCount>>(gfx, pmc, 1u));

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode( int& nextID, const aiNode& node ) noexcept
{
	const auto transform = DirectX::XMMatrixTranspose(
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

	auto pNode = std::make_unique<Node>( nextID++, node.mName.C_Str(), std::move( curMeshPtrs ), transform );
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild( ParseNode( nextID, *node.mChildren[i] ) );
	}

	return pNode;
}

Model::~Model() noexcept { }
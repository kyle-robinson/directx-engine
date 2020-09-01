#include "Node.h"
#include "Mesh.h"
#include "ModelProbe.h"
#include "imgui/imgui.h"

Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG)
	: meshPtrs(std::move(meshPtrs)), name(name), id(id)
{
	DirectX::XMStoreFloat4x4(&baseTransform, transform);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	const auto built =
		DirectX::XMLoadFloat4x4(&baseTransform) *
		DirectX::XMLoadFloat4x4(&appliedTransform) *
		accumulatedTransform;

	for (const auto pm : meshPtrs)
		pm->Submit(frame, built);

	for (const auto& pc : childPtrs)
		pc->Submit(frame, built);
}

/*void Node::RenderTree(Node*& pSelectedNode) const noexcept
{
	// set id to impossible value, if no node is selected
	const int selectedID = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetID();

	// build flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetID() == selectedID) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((childPtrs.empty()) ? ImGuiTreeNodeFlags_Leaf : 0);

	// render current node
	const auto expanded = ImGui::TreeNodeEx(
		(void*)(intptr_t)GetID(), node_flags, name.c_str()
	);

	// for selecting nodes
	if (ImGui::IsItemClicked())
		pSelectedNode = const_cast<Node*>(this);

	// if node is expanded, recursively render all children
	if (expanded)
	{
		for (const auto& pChild : childPtrs)
			pChild->RenderTree(pSelectedNode);
		ImGui::TreePop();
	}
}*/

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
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

void Node::Accept( ModelProbe& probe )
{
	if ( probe.PushNode( *this ) )
	{
		for ( auto& cp : childPtrs )
			cp->Accept( probe );

		probe.PopNode( *this );
	}
}

void Node::Accept( TechniqueProbe& probe )
{
	for ( auto& mp : meshPtrs )
		mp->Accept( probe );
}
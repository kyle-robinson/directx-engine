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

void Node::Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	const auto built =
		DirectX::XMLoadFloat4x4(&baseTransform) *
		DirectX::XMLoadFloat4x4(&appliedTransform) *
		accumulatedTransform;

	for (const auto pm : meshPtrs)
		pm->Submit(channels, built);

	for (const auto& pc : childPtrs)
		pc->Submit(channels, built);
}

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
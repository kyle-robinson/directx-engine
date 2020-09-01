#pragma once
#include "Graphics.h"

class Model;
class Mesh;
class FrameCommander;

class Node
{
	friend Model;
public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noexcept(!IS_DEBUG);
	void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetID() const noexcept;
	//void RenderTree(Node*& pSelectedNode) const noexcept;
private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);
private:
	int id;
	std::string name;
	std::vector<Mesh*> meshPtrs;
	std::vector<std::unique_ptr<Node>> childPtrs;
	DirectX::XMFLOAT4X4 baseTransform;
	DirectX::XMFLOAT4X4 appliedTransform;
};
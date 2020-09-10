#pragma once
#include "Graphics.h"

class Model;
class Mesh;
class FrameCommander;
class TechniqueProbe;
class ModelProbe;

class Node
{
	friend Model;
public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noexcept(!IS_DEBUG);
	void Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetID() const noexcept;
	bool HasChildren() const noexcept
	{
		return childPtrs.size() > 0;
	}
	void Accept( ModelProbe& probe );
	void Accept( TechniqueProbe& probe );
	const std::string& GetName() const
	{
		return name;
	}
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
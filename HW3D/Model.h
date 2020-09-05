#pragma once
#include "Graphics.h"
#include <filesystem>
#include <string>
#include <memory>

class Node;
class Mesh;
class RenderGraph;
struct aiMesh;
struct aiMaterial;
struct aiNode;

class Model
{
public:
	Model(Graphics& gfx, const std::string& pathString, float scale = 1.0f);
	void Submit() const noexcept(!IS_DEBUG);
	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
	void Accept( class ModelProbe& probe );
	void LinkTechniques( RenderGraph& );
	~Model() noexcept;
private:
	std::unique_ptr<Node> ParseNode( int& nextID, const aiNode& node, float scale ) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};
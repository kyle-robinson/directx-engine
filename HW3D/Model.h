#pragma once
#include "Graphics.h"
#include <filesystem>
#include <string>
#include <memory>

class Node;
class Mesh;
class FrameCommander;
class ModelWindow;
struct aiMesh;
struct aiMaterial;
struct aiNode;

class Model
{
public:
	Model(Graphics& gfx, const std::string& pathString, float scale = 1.0f);
	void Submit(FrameCommander& frame) const noexcept(!IS_DEBUG);
	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
	void Accept( class ModelProbe& probe );
	~Model() noexcept;
private:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	std::unique_ptr<Node> ParseNode( int& nextID, const aiNode& node, DirectX::FXMMATRIX additionalTransform ) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	//std::unique_ptr<class ModelWindow> pWindow;
};
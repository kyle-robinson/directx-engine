#pragma once
#include "DrawableBase.h"
#include "BindableCommon.h"
#include "Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh( Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs );
	void Draw( Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform ) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransformXM() const noexcept;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
public:
	Node( const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform ) noexcept(!IS_DEBUG);
	void Draw( Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform ) const noexcept(!IS_DEBUG);
	void RenderTree() const noexcept(!IS_DEBUG);
private:
	void AddChild( std::unique_ptr<Node> pChild ) noexcept(!IS_DEBUG);
private:
	std::string name;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
};

class Model
{
public:
	Model( Graphics& gfx, const std::string fileName );
	void Draw( Graphics& gfx ) const noexcept(!IS_DEBUG);
	void ShowControlWindow( const char* windowName = nullptr ) noexcept;
	~Model() noexcept;
private:
	static std::unique_ptr<Mesh> ParseMesh( Graphics& gfx, const aiMesh& mesh );
	std::unique_ptr<Node> ParseNode( const aiNode& node ) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pWindow;
};
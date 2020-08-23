#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include "ConstantBuffers.h"
#include "imgui/imgui.h"
#include <optional>
#include <type_traits>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelException : public Exception
{
public:
	ModelException( int line, const char* file, std::string note ) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;
private:
	std::string note;
};

class Mesh : public Drawable
{
public:
	Mesh( Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs );
	void Draw( Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform ) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransformXM() const noexcept;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
public:
	struct PSMaterialConstantFull
	{
		BOOL normalMapEnabled = TRUE;
		BOOL specularMapEnabled = TRUE;
		BOOL hasGlossMap = FALSE;
		float specularPower = 3.1f;
		DirectX::XMFLOAT3 specularColor = { 0.75f, 0.75f, 0.75f };
		float specularMapWeight = 0.671f;
	};
	struct PSMaterialConstantNoTexture
	{
		DirectX::XMFLOAT4 materialColor = { 0.447970f, 0.327254f, 0.176283f, 1.0f };
		float specularIntensity = 0.65f;
		float specularPower = 120.0f;
		float padding[2];
	};
public:
	Node( int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in ) noexcept(!IS_DEBUG);
	void Draw( Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform ) const noexcept(!IS_DEBUG);
	void SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept;
	int GetID() const noexcept;
	void RenderTree( Node*& pSelectedNode ) const noexcept;
	template<class T>
	bool ControlWindow( Graphics& gfx, T& c )
	{
		if (meshPtrs.empty())
		{
			ImGui::Text("No mesh selected!");
			return false;
		}

		if constexpr( std::is_same<T, PSMaterialConstantFull>::value )
		{
			if (auto pcb = meshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<T>>())
			{
				bool normalMapEnabled = (bool)c.normalMapEnabled;
				ImGui::Checkbox("Normal Map", &normalMapEnabled);
				c.normalMapEnabled = normalMapEnabled ? TRUE : FALSE;

				bool specularMapEnabled = (bool)c.specularMapEnabled;
				ImGui::Checkbox("Specular Map", &specularMapEnabled);
				c.specularMapEnabled = specularMapEnabled ? TRUE : FALSE;

				bool glossMapEnabled = (bool)c.hasGlossMap;
				ImGui::Checkbox("Gloss Alpha", &glossMapEnabled);
				c.hasGlossMap = glossMapEnabled ? TRUE : FALSE;

				ImGui::SliderFloat("Spec. Weight", &c.specularMapWeight, 0.0f, 2.0f);
				ImGui::SliderFloat("Spec. Power", &c.specularPower, 0.0f, 1000.0f, "%f", 5.0f);
				ImGui::ColorPicker3("Spec. Color", reinterpret_cast<float*>(&c.specularColor));

				pcb->Update(gfx, c);
				return true;
			}
		}
		else if constexpr( std::is_same<T, PSMaterialConstantNoTexture>::value )
		{
			if ( auto pcb = meshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<T>>() )
			{
				ImGui::SliderFloat( "Spec. Inten.", &c.specularIntensity, 0.0f, 1.0f );
				ImGui::SliderFloat( "Spec. Pow.", &c.specularPower, 0.0f, 1000.0f, "%f", 5.0f );
				ImGui::ColorPicker3( "Diff. Color", reinterpret_cast<float*>( &c.materialColor ) );

				pcb->Update( gfx, c );
				return true;
			}
		}
		return false;
	}
private:
	void AddChild( std::unique_ptr<Node> pChild ) noexcept(!IS_DEBUG);
private:
	std::string name;
	int id;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 baseTransform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:
	Model( Graphics& gfx, const std::string fileName );
	void Draw( Graphics& gfx ) const noexcept(!IS_DEBUG);
	void ShowControlWindow( Graphics& gfx, const char* windowName = nullptr ) noexcept;
	void SetRootTransform( DirectX::FXMMATRIX tf ) noexcept;
	~Model() noexcept;
private:
	static std::unique_ptr<Mesh> ParseMesh( Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials );
	std::unique_ptr<Node> ParseNode( int& nextID, const aiNode& node ) noexcept;
public:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pWindow;
};
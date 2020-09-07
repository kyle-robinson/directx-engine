#pragma once
#include "Node.h"
#include "Model.h"
#include "MathX.h"
#include "Technique.h"
#include "ModelProbe.h"
#include "TechniqueProbe.h"
#include "DynamicConstant.h"
#include "imgui/imgui.h"
#include <string>
#include <DirectXMath.h>
#include <unordered_map>

// Mesh techniques window
class TP : public TechniqueProbe
{
public:
	void OnSetTechnique() override
	{
		using namespace std::string_literals;
		ImGui::TextColored( { 0.4f, 1.0f, 0.6f, 1.0f }, pTech->GetName().c_str() );
		bool active = pTech->IsActive();
		ImGui::Checkbox( ( pTech->GetName() + std::string( " Enable" ) ).c_str(), &active );
		pTech->SetActiveState( active );
	}
	bool OnVisitBuffer( Dcb::Buffer& buf ) override
	{
		float bufferSet = false;
		const auto linkCheck = [&bufferSet]( bool changed ) { bufferSet = bufferSet || changed; };
		auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string( bufIdx )]
		( const char* label ) mutable
		{
			tagScratch = label + tagString;
			return tagScratch.c_str();
		};

		if ( auto v = buf["useSpecularMap"]; v.Exists() )
			linkCheck( ImGui::Checkbox( tag( "Spec. Map Enable" ), &v ) );
		
		if ( auto v = buf["useNormalMap"]; v.Exists() )
			linkCheck( ImGui::Checkbox( tag( "Normal Map Enable" ), &v ) );

		if ( auto v = buf["scale"]; v.Exists() )
			linkCheck( ImGui::SliderFloat( tag( "Scale"), &v, 1.0f, 2.0f, "%.3f", 3.5f ) );

		if ( auto v = buf["offset"]; v.Exists() )
			linkCheck( ImGui::SliderFloat( tag( "Offset" ), &v, 0.0f, 1.0f, "%.3f", 2.5f ) );

		if ( auto v = buf["materialColor"]; v.Exists() )
			linkCheck( ImGui::ColorPicker3( tag( "Color" ), reinterpret_cast<float*>( &static_cast<DirectX::XMFLOAT3&>( v ) ) ) );
		
		if ( auto v = buf["specularColor"]; v.Exists() )
			linkCheck( ImGui::ColorPicker3( tag( "Spec. Color" ), reinterpret_cast<float*>( &static_cast<DirectX::XMFLOAT3&>( v ) ) ) );
		
		if ( auto v = buf["specularGloss"]; v.Exists() )
			linkCheck( ImGui::SliderFloat( tag( "Glossiness" ), &v, 1.0f, 100.0f, "%.1f", 1.5f ) );
		
		if ( auto v = buf["specularWeight"]; v.Exists() )
			linkCheck( ImGui::SliderFloat( tag( "Specular Weight" ), &v, 0.0f, 2.0f ) );
		
		if ( auto v = buf["normalMapWeight"]; v.Exists() )
			linkCheck( ImGui::SliderFloat( tag( "Normal Weight" ), &v, 0.0f, 2.0f ) );

		return bufferSet;
	}
};

class MP : ModelProbe
{
public:
	void SpawnWindow( Model& model, std::string name )
	{
		if( ImGui::Begin( name.empty() ? "Model" : name.c_str() ) )
		{
			ImGui::Columns( 2, nullptr, true );
			model.Accept( *this );

			ImGui::NextColumn();
			if ( pSelectedNode != nullptr )
			{
				bool bufferSet = false;
				const auto linkCheck = [&bufferSet]( bool changed ) { bufferSet = bufferSet || changed; };
				auto& tf = ResolveTransform();

				ImGui::TextColored( { 0.4f, 1.0f, 0.6f, 1.0f }, "Translation" );
				linkCheck( ImGui::SliderFloat( "X", &tf.x, -60.f, 60.f ) );
				linkCheck( ImGui::SliderFloat( "Y", &tf.y, -60.f, 60.f ) );
				linkCheck( ImGui::SliderFloat( "Z", &tf.z, -60.f, 60.f ) );

				ImGui::TextColored( { 0.4f, 1.0f, 0.6f, 1.0f }, "Orientation" );
				linkCheck( ImGui::SliderAngle( "X-Rotation", &tf.xRot, -180.0f, 180.0f ) );
				linkCheck( ImGui::SliderAngle( "Y-Rotation", &tf.yRot, -180.0f, 180.0f ) );
				linkCheck( ImGui::SliderAngle( "Z-Rotation", &tf.zRot, -180.0f, 180.0f ) );

				if ( bufferSet )
				{
					pSelectedNode->SetAppliedTransform(
						DirectX::XMMatrixRotationX( tf.xRot ) *
						DirectX::XMMatrixRotationY( tf.yRot ) *
						DirectX::XMMatrixRotationZ( tf.zRot ) *
						DirectX::XMMatrixTranslation( tf.x, tf.y, tf.z )
					);
				}

				if ( ImGui::Button( "Reset Transform" ) )
					pSelectedNode->SetAppliedTransform( DirectX::XMMatrixIdentity() );

				TP probe;
				pSelectedNode->Accept( probe );
			}
		}
		ImGui::End();
	}
protected:
	bool PushNode( Node& node ) override
	{
		// if there is no selected node, set selectedId to an impossible value
		const int selectedId = ( pSelectedNode == nullptr ) ? -1 : pSelectedNode->GetID();
		
		// build up flags for current node
		const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ( ( node.GetID() == selectedId ) ? ImGuiTreeNodeFlags_Selected : 0 )
			| ( node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf );
		
		// render this node
		const auto expanded = ImGui::TreeNodeEx(
			(void*)(intptr_t)node.GetID(),
			node_flags, node.GetName().c_str()
		);

		// processing for selecting node
		if ( ImGui::IsItemClicked() )
			pSelectedNode = &node;

		// signal if children should also be recursed
		return expanded;
	}
	void PopNode( Node& node ) override
	{
		ImGui::TreePop();
	}
private:
	Node* pSelectedNode = nullptr;
	struct TransformParameters
	{
		float xRot = 0.0f;
		float yRot = 0.0f;
		float zRot = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> transformParams;
private:
	TransformParameters& ResolveTransform() noexcept
	{
		const auto id = pSelectedNode->GetID();
		auto i = transformParams.find( id );
		if ( i == transformParams.end() )
			return LoadTransform( id );

		return i->second;
	}
	TransformParameters& LoadTransform(int id) noexcept
	{
		const auto& applied = pSelectedNode->GetAppliedTransform();
		const auto angles = ExtractEulerAngles(applied);
		const auto translation = ExtractTranslation(applied);
		TransformParameters tp;
		tp.zRot = angles.z;
		tp.xRot = angles.x;
		tp.yRot = angles.y;
		tp.x = translation.x;
		tp.y = translation.y;
		tp.z = translation.z;
		return transformParams.insert({ id,{ tp } }).first->second;
	}
};
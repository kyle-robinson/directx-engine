#include "ModelWindow.h"
#include "Node.h"
#include <DirectXMath.h>
#include <cassert>

void ModelWindow::Show(Graphics& gfx, const char* windowName, const Node& root) noexcept
{
	/*windowName = windowName ? windowName : "Model";
	int nodeIndexTracker = 0;

	if ( ImGui::Begin( windowName ) )
	{
		ImGui::Columns( 2, nullptr, true );
		root.RenderTree( pSelectedNode );

		ImGui::NextColumn();
		if ( pSelectedNode != nullptr )
		{
			const auto id = pSelectedNode->GetID();
			auto i = transforms.find( id );
			if ( i == transforms.end() )
			{
				const auto& applied = pSelectedNode->GetAppliedTransform();
				const auto angles = ExtractEulerAngles( applied );
				const auto translation = ExtractTranslation( applied );

				TransformParameters tp;
				tp.roll = angles.z;
				tp.pitch = angles.x;
				tp.yaw = angles.y;
				tp.x = translation.x;
				tp.y = translation.y;
				tp.z = translation.z;

				auto pMatConst = pSelectedNode->GetMaterialConstants();
				auto buf = pMatConst != nullptr ? std::optional<Dcb::Buffer>{ *pMatConst } : std::optional<Dcb::Buffer>{};
				std::tie( i, std::ignore ) = transforms.insert( { id, { tp, false, std::move( buf ), false } } );
			}

			// link imgui to cached transform params
			{
				auto& transform = i->second.transformParams;
				auto& transformLink = i->second.transformParamsBool;
				const auto linkCheck = [&transformLink]( bool changed ){ transformLink = transformLink || changed; };

				if ( ImGui::CollapsingHeader( "Orientation" ) )
				{
					linkCheck( ImGui::SliderAngle( "Roll", &transform.roll, -180.0f, 180.0f ) );
					linkCheck( ImGui::SliderAngle( "Pitch", &transform.pitch, -180.0f, 180.0f ) );
					linkCheck( ImGui::SliderAngle( "Yaw", &transform.yaw, -180.0f, 180.0f ) );
				}

				if ( ImGui::CollapsingHeader( "Position" ) )
				{
					linkCheck( ImGui::SliderFloat( "X", &transform.x, -20.0f, 20.0f ) );
					linkCheck( ImGui::SliderFloat( "Y", &transform.y, -20.0f, 20.0f ) );
					linkCheck( ImGui::SliderFloat( "Z", &transform.z, -20.0f, 20.0f ) );
				}
			}

			// link imgui to cached material params
			if( i->second.materialCBuf )
			{
				auto& mat = *i->second.materialCBuf;
				auto& materialLink = i->second.materialCBufBool;
				const auto linkCheck = [&materialLink]( bool changed ){ materialLink = materialLink || changed; };

				if ( ImGui::CollapsingHeader( "Materials" ) )
				{
					if ( auto v = mat["normalMapEnabled"]; v.Exists() )
						linkCheck( ImGui::Checkbox( "Normal Map", &v ) );
					if ( auto v = mat["specularMapEnabled"]; v.Exists() )
						linkCheck( ImGui::Checkbox( "Specular Map", &v ) );
					if ( auto v = mat["hasGlossMap"]; v.Exists() )
						linkCheck( ImGui::Checkbox( "Gloss Map", &v ) );
					if ( auto v = mat["materialColor"]; v.Exists() )
						linkCheck( ImGui::ColorPicker3( "Diffuse", reinterpret_cast<float*>( &static_cast<DirectX::XMFLOAT3&>( v ) ) ) );
					if ( auto v = mat["specularPower"]; v.Exists() )
						linkCheck( ImGui::SliderFloat( "Spec. Power", &v, 0.0f, 100.0f, "%.1f", 1.5f ) );
					if ( auto v = mat["specularColor"]; v.Exists() )
						linkCheck( ImGui::ColorPicker3( "Specular", reinterpret_cast<float*>( &static_cast<DirectX::XMFLOAT3>( v ) ) ) );
					if ( auto v = mat["specularMapWeight"]; v.Exists() )
						linkCheck( ImGui::SliderFloat( "Spec. Weight", &v, 0.0f, 4.0f ) );
					if ( auto v = mat["specularIntensity"]; v.Exists() )
						linkCheck( ImGui::SliderFloat( "Spec. Inten", &v, 0.0f, 1.0f ) );
				}
			}
		}
	}
	ImGui::End();*/
}

void ModelWindow::ApplyParameters() noexcept(!IS_DEBUG)
{
	/*if ( Transform() )
	{
		pSelectedNode->SetAppliedTransform( GetTransform() );
		ResetTransform();
	}
	if ( Material() )
	{
		pSelectedNode->SetMaterialConstants( GetMaterial() );
		ResetMaterial();
	}*/
}

/*void ResetMesh() noexcept
{
	auto& transform = transforms.at( pSelectedNode->GetID() );
	transform.roll = 0.0f;
	transform.pitch = 0.0f;
	transform.yaw = 0.0f;
	transform.x = 0.0f;
	transform.y = 0.0f;
	transform.z = 0.0f;
}*/

DirectX::XMMATRIX ModelWindow::GetTransform() const noexcept(!IS_DEBUG)
{
	assert(pSelectedNode != nullptr);
	return DirectX::XMMatrixIdentity();
}

bool ModelWindow::Transform() const noexcept(!IS_DEBUG)
{
	return false;
}

void ModelWindow::ResetTransform() noexcept(!IS_DEBUG)
{
	//transforms.at(pSelectedNode->GetID()).transformParamsBool = false;
}

bool ModelWindow::Material() const noexcept(!IS_DEBUG)
{
	return false;
}

void ModelWindow::ResetMaterial() noexcept(!IS_DEBUG)
{
	//transforms.at(pSelectedNode->GetID()).materialCBufBool = false;
}

bool ModelWindow::TransformMaterial() const noexcept(!IS_DEBUG)
{
	return Transform() || Material();
}
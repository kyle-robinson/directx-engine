#include "App.h"
#include "Math.h"
#include "Mesh.h"
#include "Node.h"
#include "ModelProbe.h"
#include "DynamicConstant.h"
#include "imgui/imgui.h"
#include <memory>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

App::App() : wnd( 1280, 720, "DirectX 11 Engine Window" ), light( wnd.Gfx() )
{
	//cube.SetPos( { 4.0f, 0.0f, 0.0f } );
	//cube2.SetPos( { 0.0f, 4.0f, 0.0f } );

	/*{
		std::string path = "res\\models\\brick_wall\\brick_wall.obj";
		Assimp::Importer importer;
		const auto pScene = importer.ReadFile( path,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace
		);
		Material mat{ wnd.Gfx(), *pScene->mMaterials[1], path };
		pLoaded = std::make_unique<Mesh>( wnd.Gfx(), mat, *pScene->mMeshes[0] );
	}*/

	wnd.Gfx().SetProjection( DirectX::XMMatrixPerspectiveLH( 1.0f, 3.0f / 4.0f, 0.5f, 400.0f ) );
}

int App::Init()
{
	// handle messages
	while ( true )
	{
		if ( const auto ecode = Window::ProcessMessages() )
			return *ecode;

		DoFrame();
	}
}

App::~App() { }

void App::DoFrame()
{
	const auto dt = timer.Mark();

	if ( wnd.kbd.KeyIsPressed( VK_ESCAPE ) )
		wnd.EndWindow();

	// imgui setup
	if ( wnd.kbd.KeyIsPressed( VK_F1 ) )
		wnd.Gfx().DisableImGui();
	else
		wnd.Gfx().EnableImGui();

	// setup
	wnd.Gfx().BeginFrame( 0.07f, 0.0f, 0.12f );
	wnd.Gfx().SetCamera( camera.GetMatrix() );
	light.Bind( wnd.Gfx(), camera.GetMatrix() );

	// objects
	light.Submit( fc );
	sponza.Submit( fc );
	//goblin.Submit( fc );
	//backpack.Submit( fc );
	//cube.Submit( fc );
	//cube2.Submit( fc );
	//pLoaded->Submit( fc, DirectX::XMMatrixIdentity() );

	fc.Execute( wnd.Gfx() );

	// raw mouse input
	while ( const auto& e = wnd.kbd.ReadKey() )
	{
		if ( !e->IsPress() )
			continue;

		switch ( e->GetCode() )
		{
		case VK_INSERT:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		}
	}

	// camera movement
	if ( !wnd.CursorEnabled() )
	{
		if ( wnd.kbd.KeyIsPressed( 'W' ) )
			camera.Translate( { 0.0f, 0.0f, dt } );
		if ( wnd.kbd.KeyIsPressed( 'A' ) )
			camera.Translate( { -dt, 0.0f, 0.0f } );
		if ( wnd.kbd.KeyIsPressed( 'S' ) )
			camera.Translate( { 0.0f, 0.0f, -dt } );
		if ( wnd.kbd.KeyIsPressed( 'D' ) )
			camera.Translate( { dt, 0.0f, 0.0f } );
		if ( wnd.kbd.KeyIsPressed( 'R' ) )
			camera.Translate( { 0.0f, dt, 0.0f } );
		if ( wnd.kbd.KeyIsPressed( 'F' ) )
			camera.Translate( { 0.0f, -dt, 0.0f } );
	}

	// camera rotation
	while ( const auto delta = wnd.mouse.ReadRawDelta() )
	{
		if ( !wnd.CursorEnabled() )
			camera.Rotate( delta->x, delta->y );
	}

	class TP : public TechniqueProbe
	{
	public:
		void OnSetTechnique() override
		{
			using namespace std::string_literals;
			ImGui::TextColored( { 0.4f, 1.0f, 0.6f, 1.0f }, pTech->GetName().c_str() );
			bool active = pTech->IsActive();
			ImGui::Checkbox( ( "Tech Active##"s + std::to_string( techIdx ) ).c_str(), &active );
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

			if ( auto v = buf["scale"]; v.Exists() )
				linkCheck( ImGui::SliderFloat( tag( "Scale" ), &v, 1.0f, 2.0f, "%.3f", 3.5f ) );
			if ( auto v = buf["materialColor"]; v.Exists() )
				linkCheck( ImGui::ColorPicker3( tag( "Color" ), reinterpret_cast<float*>( &static_cast<DirectX::XMFLOAT3&>( v ) ) ) );
			if ( auto v = buf["specularColor"]; v.Exists() )
				linkCheck( ImGui::ColorPicker3( tag( "Spec. Color" ), reinterpret_cast<float*>( &static_cast<DirectX::XMFLOAT3&>( v ) ) ) );
			if ( auto v = buf["specularGloss"]; v.Exists() )
				linkCheck( ImGui::SliderFloat( tag( "Spec. Gloss" ), &v, 1.0f, 100.0f, "%.1f", 1.5f ) );
			if ( auto v = buf["specularWeight"]; v.Exists() )
				linkCheck( ImGui::SliderFloat( tag( "Spec. Weight" ), &v, 0.0f, 2.0f ) );
			if ( auto v = buf["useNormalMap"]; v.Exists() )
				linkCheck( ImGui::Checkbox( tag( "Normal Map" ), &v ) );
			if ( auto v = buf["normalMapWeight"]; v.Exists() )
				linkCheck( ImGui::SliderFloat( tag( "Normal Map Weight" ), &v, 1.0f, 2.0f ) );

			return bufferSet;
		}
	} probe;
	
	class MP : public ModelProbe
	{
	public:
		void SpawnWindow( Model& model )
		{
			if ( ImGui::Begin( "Model", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
			{
				ImGui::Columns( 2, nullptr, true );
				model.Accept( *this );
				ImGui::NextColumn();
				if ( pSelectedNode != nullptr )
				{

				}
				ImGui::End();
			}
		}
	protected:
		bool PushNode( Node& node ) override
		{
			// if no node selected, set id to non value
			const int selectedId = ( pSelectedNode == nullptr ) ? -1 : pSelectedNode->GetID();
			const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
				( ( node.GetID() == selectedId ) ? ImGuiTreeNodeFlags_Selected : 0 ) |
				( node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf );

			// render node
			const auto expanded = ImGui::TreeNodeEx(
				(void*)(intptr_t)node.GetID(),
				node_flags, node.GetName().c_str()
			);

			// processing for selected node
			if ( ImGui::IsItemClicked() )
				pSelectedNode = &node;

			// signal if children should be recursed
			return expanded;
		}
		void PopNode( Node& node ) override
		{
			ImGui::TreePop();
		}
	protected:
		Node* pSelectedNode = nullptr;
	};
	static MP modelProbe;

	// imgui
	if ( wnd.Gfx().IsImGuiEnabled() )
	{
		camera.SpawnControlWindow();
		light.SpawnControlWindow();
		modelProbe.SpawnWindow( sponza );
		//cube.SpawnControlWindow( wnd.Gfx(), "Cube 1" );
		//cube2.SpawnControlWindow( wnd.Gfx(), "Cube 2" );
		ShowRawInputWindow();
	}
	
	wnd.Gfx().EndFrame();
	fc.Reset();
}

void App::ShowRawInputWindow()
{
	while ( const auto d = wnd.mouse.ReadRawDelta() )
	{
		x += d->x;
		y += d->y;
	}
	if ( ImGui::Begin( "Raw Input" ) )
	{
		ImGui::Text( "Tally: (%d,%d)", x, y );
		ImGui::Text( "Cursor: %s", wnd.CursorEnabled() ? "Enabled" : "Disabled" );
	}
	ImGui::End();
}
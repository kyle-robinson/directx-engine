#include "NormalCube.h"
#include "BindableCommon.h"
#include "Cube.h"
#include "NullPixelShader.h"
#include "TransformCbufDouble.h"
#include "imgui/imgui.h"

NormalCube::NormalCube( Graphics& gfx, float size )
{
	using namespace Bind;

	// bindables
	auto model = Cube::MakeIndependentTextured();
	model.Transform( DirectX::XMMatrixScaling( size, size, size ) );
	model.SetNormalsIndependentFlat();
	const auto geometryTag = "$cube." + std::to_string( size );
	pVertices = VertexBuffer::Resolve( gfx, geometryTag, model.vertices );
	pIndices = IndexBuffer::Resolve( gfx, geometryTag, model.indices );
	pTopology = Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	{
		Technique standard;
		{
			Step initial( 0 );

			initial.AddBindable( Texture::Resolve( gfx, "res\\textures\\brickwall.jpg" ) );
			initial.AddBindable( Sampler::Resolve( gfx ) );

			auto pvs = VertexShader::Resolve( gfx, "PhongVS.cso" );
			auto pvsbc = pvs->GetByteCode();
			initial.AddBindable( std::move( pvs ) );
			initial.AddBindable( PixelShader::Resolve( gfx, "PhongPS.cso" ) );

			initial.AddBindable( PixelConstantBuffer<PSMaterialConstant>::Resolve( gfx, pmc, 1u ) );
			initial.AddBindable( InputLayout::Resolve( gfx, model.vertices.GetLayout(), pvsbc ) );
			initial.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

			standard.AddStep( std::move( initial ) );
		}
		AddTechnique( std::move( standard ) );
	}

	{
		Technique outline;
		{
			Step mask( 1 );

			auto pvs = VertexShader::Resolve( gfx, "SolidVS.cso" );
			auto pvsbc = pvs->GetByteCode();
			mask.AddBindable( std::move( pvs ) );

			mask.AddBindable( InputLayout::Resolve( gfx, model.vertices.GetLayout(), pvsbc ) );
			mask.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

			outline.AddStep( std::move( mask ) );
		}
		{
			Step draw( 2 );

			auto pvs = VertexShader::Resolve( gfx, "SolidVS.cso" );
			auto pvsbc = pvs->GetByteCode();
			draw.AddBindable( std::move( pvs ) );
			draw.AddBindable( PixelShader::Resolve( gfx, "SolidPS.cso" ) );

			draw.AddBindable( InputLayout::Resolve( gfx, model.vertices.GetLayout(), pvsbc ) );

			class TransformCbufScaling : public TransformCbuf
			{
			public:
				using TransformCbuf::TransformCbuf;
				void Bind( Graphics& gfx ) noexcept override
				{
					const auto scale = DirectX::XMMatrixScaling( 1.03f, 1.03f, 1.03f );
					auto xf = GetTransforms( gfx );
					xf.modelView = xf.modelView * scale;
					xf.modelViewProj = xf.modelViewProj * scale;
					UpdateBind( gfx, xf );
				}
			};
			draw.AddBindable( std::make_shared<TransformCbufScaling>( gfx ) );

			outline.AddStep( std::move( draw ) );
		}
		AddTechnique( std::move( outline ) );
	}

	/*AddBind( Texture::Resolve( gfx, "res\\textures\\brickwall.jpg" ) );
	//AddBind( Texture::Resolve( gfx, "res\\textures\\brickwall_normal.jpg", 1u ) );
	AddBind( Sampler::Resolve( gfx ) );

	auto pvs = VertexShader::Resolve( gfx, "PhongVS.cso" );
	auto pvsbc = pvs->GetByteCode();
	AddBind( std::move( pvs ) );
	AddBind( PixelShader::Resolve( gfx, "PhongPS.cso" ) );

	AddBind( PixelConstantBuffer<PSMaterialConstant>::Resolve( gfx, pmc, 1u ) );

	AddBind( InputLayout::Resolve( gfx, model.vertices.GetLayout(), pvsbc ) );
	
	AddBind( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	auto tcbdb = std::make_shared<TransformCbufDouble>( gfx, *this, 0u, 2u );
	AddBind( tcbdb );

	AddBind( std::make_shared<Stencil>( gfx, Stencil::Mode::Write ) );

	// stencil bindables
	outlineEffect.push_back( VertexBuffer::Resolve( gfx, geometryTag, model.vertices ) );
	outlineEffect.push_back( IndexBuffer::Resolve( gfx, geometryTag, model.indices ) );

	pvs = VertexShader::Resolve( gfx, "SolidVS.cso" );
	pvsbc = pvs->GetByteCode();
	outlineEffect.push_back( std::move( pvs ) );
	outlineEffect.push_back( PixelShader::Resolve( gfx, "SolidPS.cso" ) );

	struct SolidColorBuffer
	{
		DirectX::XMFLOAT4 color = { 1.0f, 0.4f, 0.4f, 1.0f };
	} scb;
	outlineEffect.push_back( PixelConstantBuffer<SolidColorBuffer>::Resolve( gfx, scb, 1u ) );
	outlineEffect.push_back( InputLayout::Resolve( gfx, model.vertices.GetLayout(), pvsbc ) );
	outlineEffect.push_back( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	outlineEffect.push_back( std::move( tcbdb ) );
	outlineEffect.push_back( std::make_shared<Stencil>( gfx, Stencil::Mode::Mask ) );*/
}

void NormalCube::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void NormalCube::SetRotation( float roll, float pitch, float yaw ) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX NormalCube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( roll, pitch, yaw ) *
		DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}

void NormalCube::SpawnControlWindow( Graphics& gfx, const char* name ) noexcept
{
	name = name ? name : "Cube";
	if ( ImGui::Begin( name, FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		if ( ImGui::CollapsingHeader( "Position" ) )
		{
			ImGui::SliderFloat( "X", &pos.x, -80.0f, 80.0f, "%.1f" );
			ImGui::SliderFloat( "Y", &pos.y, -80.0f, 80.0f, "%.1f" );
			ImGui::SliderFloat( "Z", &pos.z, -80.0f, 80.0f, "%.1f" );
		}

		if ( ImGui::CollapsingHeader( "Orientation" ) )
		{
			ImGui::SliderAngle( "Roll", &roll, -180.0f, 180.0f );
			ImGui::SliderAngle( "Pitch", &pitch, -180.0f, 180.0f );
			ImGui::SliderAngle( "Yaw", &yaw, -180.0f, 180.0f );
		}

		/*if ( ImGui::CollapsingHeader( "Shading" ) )
		{
			bool specInt = ImGui::SliderFloat( "Spec. Int", &pmc.specularIntensity, 0.0f, 1.0f );
			bool specPow = ImGui::SliderFloat( "Spec. Pow", &pmc.specularPower, 0.0f, 100.0f );
			bool checkState = pmc.normalMapEnabled == TRUE;
			bool normalEnabled = ImGui::Checkbox( "Normal Mapping", &checkState );
			pmc.normalMapEnabled = checkState ? TRUE : FALSE;

			if ( specInt || specPow || normalEnabled )
				QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update( gfx, pmc );
		}*/
	}
	ImGui::End();
}

/*void NormalCube::DrawOutline( Graphics& gfx ) noexcept(!IS_DEBUG)
{
	outlining = true;
	for ( auto& b : outlineEffect )
		b->Bind( gfx );
	gfx.DrawIndexed( QueryBindable<Bind::IndexBuffer>()->GetCount() );
	outlining = false;
}*/
#include "NormalCube.h"
#include "BindableCommon.h"
#include "Cube.h"
#include "TransformCbufDouble.h"
#include "ConstantBufferEx.h"
#include "DynamicConstant.h"
#include "TechniqueProbe.h"
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
		Technique shade( "Shading" );
		{
			Step initial( 0 );

			initial.AddBindable( Texture::Resolve( gfx, "res\\textures\\brickwall.jpg" ) );
			initial.AddBindable( Sampler::Resolve( gfx ) );

			auto pvs = VertexShader::Resolve( gfx, "PhongVS.cso" );
			auto pvsbc = pvs->GetByteCode();
			initial.AddBindable( std::move( pvs ) );
			initial.AddBindable( PixelShader::Resolve( gfx, "PhongPS.cso" ) );

			Dcb::RawLayout layout;
			layout.Add<Dcb::Float>( "specularIntensity" );
			layout.Add<Dcb::Float>( "specularPower" );
			
			auto buf = Dcb::Buffer( std::move( layout ) );
			buf["specularIntensity"] = 0.1f;
			buf["specularPower"] = 20.0f;
			initial.AddBindable( std::make_shared<Bind::CachingPixelConstantBufferEx>( gfx, buf, 1u ) );

			initial.AddBindable( InputLayout::Resolve( gfx, model.vertices.GetLayout(), pvsbc ) );
			initial.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

			shade.AddStep( std::move( initial ) );
		}
		AddTechnique( std::move( shade ) );
	}

	{
		Technique outline( "Outline" );
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

			Dcb::RawLayout layout;
			layout.Add<Dcb::Float4>( "color" );

			auto buf = Dcb::Buffer( std::move( layout ) );
			buf["color"] = DirectX::XMFLOAT4{ 1.0f, 0.4f, 0.4f, 1.0f };
			draw.AddBindable( std::make_shared<Bind::CachingPixelConstantBufferEx>( gfx, buf, 1u ) );

			draw.AddBindable( InputLayout::Resolve( gfx, model.vertices.GetLayout(), pvsbc ) );

			class TransformCbufScaling : public TransformCbuf
			{
			public:
				TransformCbufScaling( Graphics& gfx, float scale = 1.03f ) : TransformCbuf( gfx ), buf( MakeLayout() )
				{
					buf["scale"] = scale;
				}
				void Accept( TechniqueProbe& probe ) override
				{
					probe.VisitBuffer( buf );
				}
				void Bind( Graphics& gfx ) noexcept override
				{
					const float scale = buf["scale"];
					const auto scaleMatrix = DirectX::XMMatrixScaling( scale, scale, scale );
					auto xf = GetTransforms( gfx );
					xf.modelView = xf.modelView * scaleMatrix;
					xf.modelViewProj = xf.modelViewProj * scaleMatrix;
					UpdateBind( gfx, xf );
				}
			private:
				static Dcb::RawLayout MakeLayout()
				{
					Dcb::RawLayout layout;
					layout.Add<Dcb::Float>( "scale" );
					return layout;
				}
			private:
				Dcb::Buffer buf;
			};
			draw.AddBindable( std::make_shared<TransformCbufScaling>( gfx ) );

			outline.AddStep( std::move( draw ) );
		}
		AddTechnique( std::move( outline ) );
	}
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

		if ( ImGui::CollapsingHeader( "Shading" ) )
		{
			class Probe : public TechniqueProbe
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
					if ( auto v = buf["color"]; v.Exists() )
						linkCheck( ImGui::ColorPicker4( tag( "Color" ), reinterpret_cast<float*>( &static_cast<DirectX::XMFLOAT4&>( v ) ) ) );
					if ( auto v = buf["specularIntensity"]; v.Exists() )
						linkCheck( ImGui::SliderFloat( tag( "Spec. Inten" ), &v, 0.0f, 1.0f ) );
					if ( auto v = buf["specularPower"]; v.Exists() )
						linkCheck( ImGui::SliderFloat( tag( "Glossiness" ), &v, 1.0f, 100.0f, "%.1f", 1.5f ) );

					return bufferSet;
				}
			} probe;
			Accept( probe );
		}
	}
	ImGui::End();
}
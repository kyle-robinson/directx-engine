#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"

Box::Box( Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist )
	:
	r( rdist( rng ) ),
	droll( ddist( rng ) ),
	dpitch( ddist( rng ) ),
	dyaw( ddist( rng ) ),
	dphi( odist( rng ) ),
	dtheta( odist( rng ) ),
	dchi( odist( rng ) ),
	chi( adist( rng ) ),
	theta( adist( rng ) ),
	phi( adist( rng ) )
{
	if ( !IsStaticInitialised() )
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
		};

		/*const std::vector<Vertex> vertices =
		{
			{ -1.0f, -1.0f, -1.0f },
			{  1.0f, -1.0f, -1.0f },
			{ -1.0f,  1.0f, -1.0f },
			{  1.0f,  1.0f, -1.0f },
			{ -1.0f, -1.0f,  1.0f },
			{  1.0f, -1.0f,  1.0f },
			{ -1.0f,  1.0f,  1.0f },
			{  1.0f,  1.0f,  1.0f }
		};*/
		const auto model = Cube::Make<Vertex>();
		//model.Transform( DirectX::XMMatrixScaling( 1.0f, 1.0f, 1.2f ) );
		AddStaticBind( std::make_unique<VertexBuffer>( gfx, model.vertices ) );

		auto pvs = std::make_unique<VertexShader>( gfx, L"ColorIndexVS.cso" );
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx, L"ColorIndexPS.cso" ) );

		/*const std::vector<unsigned short> indices =
		{
			0, 2, 1,	2, 3, 1,
			1, 3, 5,	3, 7, 5,
			2, 6, 3,	3, 6, 7,
			4, 5, 7,	4, 7, 6,
			0, 4, 2,	2, 4, 6,
			0, 1, 4,	1, 5, 4
		};*/
		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx, model.indices ) );

		struct PixelShaderConstants
		{
			struct
			{
				float r, g, b, a;
			} face_colors[6];
		};

		const PixelShaderConstants cb2 =
		{
			{
				{ 1.0f, 0.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 0.0f },
				{ 0.0f, 1.0f, 1.0f }
			}
		};
		AddStaticBind( std::make_unique<PixelConstantBuffer<PixelShaderConstants>>( gfx, cb2 ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx, ied, pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<TransformCbuf>( gfx, *this ) );

	// model deformation transform
	DirectX::XMStoreFloat3x3(
		&mt, DirectX::XMMatrixScaling( 1.0f, 1.0f, bdist( rng ) )
	);
}

void Box::Update( float dt ) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3( &mt ) *
		DirectX::XMMatrixTranslation( r, 0.0f, 0.0f ) *
		DirectX::XMMatrixRotationRollPitchYaw( theta, phi, chi ) *
		DirectX::XMMatrixTranslation( 0.0f, 0.0f, 20.0f );
}
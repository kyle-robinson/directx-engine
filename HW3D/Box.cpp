#include "Box.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"

Box::Box(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 material)
	:
	PrimitiveObject( gfx, rng, adist, ddist, odist, rdist )
{
	if ( !IsStaticInitialised() )
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};

		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();
		AddStaticBind( std::make_unique<Bind::VertexBuffer>( gfx, model.vertices ) );

		auto pvs = std::make_unique<Bind::VertexShader>( gfx, L"PhongVS.cso" );
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind( std::move( pvs ) );
		AddStaticBind( std::make_unique<Bind::PixelShader>( gfx, L"PhongPS.cso" ) );
		AddStaticIndexBuffer( std::make_unique<Bind::IndexBuffer>( gfx, model.indices ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		AddStaticBind( std::make_unique<Bind::InputLayout>( gfx, ied, pvsbc ) );
		AddStaticBind( std::make_unique<Bind::Topology>( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<Bind::TransformCbuf>( gfx, *this ) );

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} colorConst;
	colorConst.color = material;
	AddBind( std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>( gfx, colorConst, 1u ) );

	// model deformation transform
	DirectX::XMStoreFloat3x3(
		&mt, DirectX::XMMatrixScaling( 1.0f, 1.0f, bdist( rng ) )
	);
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3( &mt ) * PrimitiveObject::GetTransformXM();
}
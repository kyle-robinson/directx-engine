#include "SolidSphere.h"
#include "Sphere.h"
#include "Vertex.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"

SolidSphere::SolidSphere( Graphics& gfx, float radius )
{
	using namespace Bind;

	auto model = Sphere::Make();
	model.Transform( DirectX::XMMatrixScaling( radius, radius, radius ) );
	AddBind( std::make_shared<VertexBuffer>( gfx, model.vertices ) );
	AddBind( std::make_shared<IndexBuffer>( gfx, model.indices ) );

	auto pvs = std::make_shared<VertexShader>( gfx, "SolidVS.cso" );
	auto pvsbc = pvs->GetByteCode();
	AddBind( std::move( pvs ) );
	AddBind( std::make_shared<PixelShader>( gfx, L"SolidPS.cso" ) );

	struct PSColorConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float padding;
	} colorConst;
	AddBind( std::make_shared<PixelConstantBuffer<PSColorConstant>>( gfx, colorConst ) );

	AddBind( std::make_shared<InputLayout>( gfx, model.vertices.GetLayout().GetD3DLayout(), pvsbc ) );

	AddBind( std::make_shared<Topology>( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<TransformCbuf>( gfx, *this ) );
}

void SolidSphere::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}
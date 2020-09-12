#include "SkyboxTransformCbuf.h"

namespace Bind
{
	SkyboxTransformCbuf::SkyboxTransformCbuf( Graphics& gfx, UINT slot ) :
		pVcbuf{ std::make_unique<VertexConstantBuffer<Transforms>>( gfx, slot ) }
	{ }

	void SkyboxTransformCbuf::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		INFOMANAGER_NOHR( gfx );
		GFX_THROW_INFO_ONLY( UpdateBind( gfx, GetTransforms( gfx ) ) );
	}

	void SkyboxTransformCbuf::UpdateBind( Graphics& gfx, const Transforms& tf ) noexcept(!IS_DEBUG)
	{
		pVcbuf->Update( gfx, tf );
		pVcbuf->Bind( gfx );
	}

	SkyboxTransformCbuf::Transforms SkyboxTransformCbuf::GetTransforms( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		return { DirectX::XMMatrixTranspose( gfx.GetCamera() * gfx.GetProjection() ) };
	}
}
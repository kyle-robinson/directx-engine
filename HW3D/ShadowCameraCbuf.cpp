#include "ShadowCameraCbuf.h"
#include "Camera.h"

namespace Bind
{
	ShadowCameraCbuf::ShadowCameraCbuf( Graphics& gfx, UINT slot ) :
		pVcbuf{ std::make_unique<VertexConstantBuffer<Transform>>( gfx, slot ) }
	{ }

	void ShadowCameraCbuf::Bind( Graphics& gfx ) noexcept(!IS_DEBUG)
	{
		pVcbuf->Bind( gfx );
	}

	void ShadowCameraCbuf::Update( Graphics& gfx )
	{
		const Transform transform{
			DirectX::XMMatrixTranspose(
				pCamera->GetMatrix() * pCamera->GetProjection()
			)
		};
		pVcbuf->Update( gfx, transform );
	}

	void ShadowCameraCbuf::SetCamera( const Camera* pCam ) noexcept
	{
		pCamera = pCam;
	}
}
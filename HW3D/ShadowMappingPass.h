#pragma once
#include "RenderQueuePass.h"
#include "Blender.h"
#include "NullPixelShader.h"
#include "ShadowRasterizer.h"
#include "Rasterizer.h"
#include "Stencil.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "RenderTarget.h"
#include "CubeTexture.h"
#include "Viewport.h"
#include "Source.h"
#include "Job.h"
#include "Math.h"
#include <vector>

class Graphics;

namespace Rgph
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		ShadowMappingPass( Graphics& gfx, std::string name ) :
			RenderQueuePass( std::move( name ) )
		{
			depthStencil = std::make_shared<Bind::OutputOnlyDepthStencil>( gfx, size, size );
			pDepthCube = std::make_shared<Bind::CubeTargetTexture>( gfx, size, size, 3, DXGI_FORMAT_R32_FLOAT );
			AddBind( Bind::VertexShader::Resolve( gfx, "ShadowCubeVS.cso" ) );
			AddBind( Bind::PixelShader::Resolve( gfx, "ShadowCubePS.cso" ) );
			AddBind( Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Off ) );
			AddBind( Bind::Blender::Resolve( gfx, false ) );
			AddBind( std::make_shared<Bind::Viewport>( gfx, static_cast<float>( size ), static_cast<float>( size ) ) );
			AddBind( std::make_shared<Bind::Rasterizer>( gfx, false ) );
			RegisterSource( DirectBindableSource<Bind::CubeTargetTexture>::Make( "map", pDepthCube ) );

			DirectX::XMStoreFloat4x4(
				&projection, DirectX::XMMatrixPerspectiveFovLH( PI / 2.0f, 1.0f, 0.5f, 100.0f )
			);

			// +x - right
			DirectX::XMStoreFloat3( &cameraDirections[0], DirectX::XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[0], DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
			// -x - left
			DirectX::XMStoreFloat3( &cameraDirections[1], DirectX::XMVectorSet( -1.0f, 0.0f, 0.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[1], DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
			// +y - up
			DirectX::XMStoreFloat3( &cameraDirections[2], DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[2], DirectX::XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f ) );
			// -y - down
			DirectX::XMStoreFloat3( &cameraDirections[3], DirectX::XMVectorSet( 0.0f, -1.0f, 0.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[3], DirectX::XMVectorSet( 0.0f, 1.0f, 1.0f, 0.0f ) );
			// +z - front
			DirectX::XMStoreFloat3( &cameraDirections[4], DirectX::XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[4], DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
			// -z - back
			DirectX::XMStoreFloat3( &cameraDirections[5], DirectX::XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[5], DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
		}
		void BindShadowCamera( const Camera& cam ) noexcept
		{
			pShadowCamera = &cam;
		}
		void Execute( Graphics& gfx ) const noexcept(!IS_DEBUG) override
		{
			DirectX::XMFLOAT3 shadowCamPos = pShadowCamera->GetPosition();
			const auto pos = DirectX::XMLoadFloat3( &shadowCamPos );
			gfx.SetProjection( DirectX::XMLoadFloat4x4( &projection ) );

			for ( size_t i = 0; i < 6; i++ )
			{
				auto rt = pDepthCube->GetRenderTarget( i );
				rt->Clear( gfx );
				depthStencil->Clear( gfx );

				SetRenderTarget( std::move( rt ) );

				const auto lookAt = DirectX::XMVectorAdd( pos, DirectX::XMLoadFloat3( &cameraDirections[i] ) );
				gfx.SetCamera( DirectX::XMMatrixLookAtLH( pos, lookAt, DirectX::XMLoadFloat3( &cameraUps[i] ) ) );

				RenderQueuePass::Execute( gfx );
			}
		}
	private:
		void SetRenderTarget( std::shared_ptr<Bind::RenderTarget> rt ) const
		{
			const_cast<ShadowMappingPass*>( this )->renderTarget = std::move( rt );
		}
	private:
		const Camera* pShadowCamera = nullptr;
		static constexpr UINT size = 1000;
		std::shared_ptr<Bind::CubeTargetTexture> pDepthCube;
		DirectX::XMFLOAT4X4 projection;
		std::vector<DirectX::XMFLOAT3> cameraDirections{ 6 };
		std::vector<DirectX::XMFLOAT3> cameraUps{ 6 };
	};
}
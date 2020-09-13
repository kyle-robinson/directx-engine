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
			pDepthCube = std::make_shared<Bind::DepthCubeTexture>( gfx, size, 3 );
			AddBind( Bind::VertexShader::Resolve( gfx, "ShadowCubeVS.cso" ) );
			//AddBind( Bind::PixelShader::Resolve( gfx, "ShadowCubePS.cso" ) );
			AddBind( Bind::NullPixelShader::Resolve( gfx ) );
			AddBind( Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Off ) );
			AddBind( Bind::Blender::Resolve( gfx, false ) );
			AddBind( std::make_shared<Bind::Viewport>( gfx, static_cast<float>( size ), static_cast<float>( size ) ) );
			AddBind( std::make_shared<Bind::ShadowRasterizer>( gfx, 50, 2.0f, 0.1f ) );
			RegisterSource( DirectBindableSource<Bind::DepthCubeTexture>::Make( "map", pDepthCube ) );

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

			SetDepthBuffer( pDepthCube->GetDepthBuffer( 0 ) );
		}
		void BindShadowCamera( const Camera& cam ) noexcept
		{
			pShadowCamera = &cam;
		}
		void Execute( Graphics& gfx ) const noexcept(!IS_DEBUG) override
		{
			const auto pos = DirectX::XMLoadFloat3( &pShadowCamera->GetPosition() );
			gfx.SetProjection( DirectX::XMLoadFloat4x4( &projection ) );

			for ( size_t i = 0; i < 6; i++ )
			{
				auto depthBuffer = pDepthCube->GetDepthBuffer( i );
				depthBuffer->Clear( gfx );

				SetDepthBuffer( std::move( depthBuffer ) );

				const auto lookAt = DirectX::XMVectorAdd( pos, DirectX::XMLoadFloat3( &cameraDirections[i] ) );
				gfx.SetCamera( DirectX::XMMatrixLookAtLH( pos, lookAt, DirectX::XMLoadFloat3( &cameraUps[i] ) ) );

				RenderQueuePass::Execute( gfx );
			}
		}
		void DumpShadowMap( Graphics& gfx, const std::string& path ) const
		{
			for ( size_t i = 0; i < 6; i++ )
			{
				auto depthBuffer = pDepthCube->GetDepthBuffer( i );
				depthBuffer->ToSurface( gfx ).Save( path + std::to_string( i ) + ".png" );
			}
		}
	private:
		void SetDepthBuffer( std::shared_ptr<Bind::DepthStencil> ds ) const
		{
			const_cast<ShadowMappingPass*>( this )->depthStencil = std::move( ds );
		}
	private:
		const Camera* pShadowCamera = nullptr;
		static constexpr UINT size = 1000;
		std::shared_ptr<Bind::DepthCubeTexture> pDepthCube;
		DirectX::XMFLOAT4X4 projection;
		std::vector<DirectX::XMFLOAT3> cameraDirections{ 6 };
		std::vector<DirectX::XMFLOAT3> cameraUps{ 6 };
	};
}
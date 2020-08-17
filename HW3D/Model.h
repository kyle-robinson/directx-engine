#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include "Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh( Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs )
	{
		if ( !IsStaticInitialised() )
		{
			AddStaticBind( std::make_unique<Topology>( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
		}

		for ( auto& pb : bindPtrs )
		{
			if ( auto pi = dynamic_cast<IndexBuffer*>( pb.get() ) )
			{
				AddIndexBuffer( std::unique_ptr<IndexBuffer>{ pi } );
				pb.release();
			}
			else
			{
				AddBind( std::move( pb ) );
			}
		}
	}
	void Draw( Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform ) const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4( &transform, accumulatedTransform );
		Drawable::Draw( gfx );
	}
	DirectX::XMMATRIX GetTransformXM() const noexcept override
	{
		return DirectX::XMLoadFloat4x4( &transform );
	}
private:
	mutable DirectX::XMFLOAT4X4 transform;
};
#pragma once
#include "IndexedTriangleList.h"
#include "Vertex.h"
#include "Math.h"
#include <DirectXMath.h>
#include <optional>

class Sphere
{
public:
	static IndexedTriangleList MakeTesselated( VertexMeta::VertexLayout layout, int latDiv, int longDiv )
	{
		assert( latDiv >= 3 );
		assert( longDiv >= 3 );

		constexpr float radius = 1.0f;
		const auto base = DirectX::XMVectorSet( 0.0f, 0.0f, radius, 0.0f );
		const float lattitudeAngle = PI / latDiv;
		const float longitudeAngle = 2.0f * PI / longDiv;

		VertexMeta::VertexBuffer vb{ std::move( layout ) };
		for ( int iLat = 1; iLat < latDiv; iLat++ )
		{
			const auto latBase = DirectX::XMVector3Transform(
				base, DirectX::XMMatrixRotationX( lattitudeAngle * iLat )
			);
			for ( int iLong = 0; iLong < longDiv; iLong++ )
			{
				DirectX::XMFLOAT3 calculatedPos;
				auto v = DirectX::XMVector3Transform(
					latBase, DirectX::XMMatrixRotationZ( longitudeAngle * iLong )
				);
				DirectX::XMStoreFloat3( &calculatedPos, v );
				vb.EmplaceBack( calculatedPos );
			}
		}

		// add cap vertices
		const auto iNorthPole = (unsigned short)vb.Size();
		{
			DirectX::XMFLOAT3 northPos;
			DirectX::XMStoreFloat3( &northPos, base );
			vb.EmplaceBack( northPos );
		}
		const auto iSouthPole = (unsigned short)vb.Size();
		{
			DirectX::XMFLOAT3 southPos;
			DirectX::XMStoreFloat3( &southPos, DirectX::XMVectorNegate( base ) );
			vb.EmplaceBack( southPos );
		}

		const auto calcIdx = [latDiv, longDiv](unsigned short iLat, unsigned short iLong)
		{ return iLat * longDiv + iLong; };
		std::vector<unsigned short> indices;
		for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
		{
			for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
			{
				indices.push_back(calcIdx(iLat, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong + 1));
			}
			// wrap band
			indices.push_back(calcIdx(iLat, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, 0));
		}

		// cap fans
		for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
		{
			// north
			indices.push_back(iNorthPole);
			indices.push_back(calcIdx(0, iLong));
			indices.push_back(calcIdx(0, iLong + 1));
			// south
			indices.push_back(calcIdx(latDiv - 2, iLong + 1));
			indices.push_back(calcIdx(latDiv - 2, iLong));
			indices.push_back(iSouthPole);
		}
		// wrap triangles
		// north
		indices.push_back(iNorthPole);
		indices.push_back(calcIdx(0, longDiv - 1));
		indices.push_back(calcIdx(0, 0));
		// south
		indices.push_back(calcIdx(latDiv - 2, 0));
		indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
		indices.push_back(iSouthPole);

		return { std::move( vb ), std::move( indices ) };
	}
	static IndexedTriangleList Make( std::optional<VertexMeta::VertexLayout> layout = std::nullopt )
	{
		if ( !layout )
		{
			layout = VertexMeta::VertexLayout{}.Append( VertexMeta::VertexLayout::ElementType::Position3D );
		}
		return MakeTesselated( std::move( *layout ), 12, 24 );
	}
};
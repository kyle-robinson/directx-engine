#pragma once
#include "Frustum.h"
#include <DirectXMath.h>

class Graphics;

namespace Rgph
{
	class RenderGraph;
}

class Projection
{
public:
	Projection( Graphics& gfx, float initialWidth, float initialHeight, float initialNearZ, float initialFarZ );
	DirectX::XMMATRIX GetMatrix() const;
	void RenderWidgets( Graphics& gfx );
	void SetPosition( DirectX::XMFLOAT3 pos );
	void SetRotation( DirectX::XMFLOAT3 rot );
	void LinkTechniques( Rgph::RenderGraph& rg );
	void Submit() const;
	void Reset();
private:
	float width, height, nearZ, farZ;
	float initialWidth, initialHeight, initialNearZ, initialFarZ;
	Frustum frust;
};
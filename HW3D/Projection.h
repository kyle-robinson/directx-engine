#pragma once
#include <DirectXMath.h>

class Projection
{
public:
	Projection( float initialWidth, float initialHeight, float initialNearZ, float initialFarZ );
	DirectX::XMMATRIX GetMatrix() const;
	void RenderWidgets();
	void Reset();
private:
	float width, height, nearZ, farZ;
	float initialWidth, initialHeight, initialNearZ, initialFarZ;
};
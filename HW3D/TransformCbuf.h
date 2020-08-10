#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"

class TransformCbuf : public Bindable
{
public:
	TransformCbuf( Graphics& gfx, const Drawable& parent );
	void Bind( Graphics& gfx ) noexcept override;
protected:
	VertexConstantBuffer<DirectX::XMMATRIX> vcbuf;
	const Drawable& parent;
};
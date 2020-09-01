#pragma once
#include "Graphics.h"
#include "Drawable.h"

class Material;
class FrameCommander;
struct aiMesh;

class Mesh : public Drawable
{
public:
	Mesh( Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f ) noexcept(!IS_DEBUG);
	void Submit( FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform ) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};
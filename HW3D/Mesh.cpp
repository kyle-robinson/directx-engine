#include "Mesh.h"
#include "Surface.h"
#include "MathX.h"
#include <unordered_map>
#include <sstream>
#include <iostream>

#include "DynamicConstant.h"
#include "ConstantBufferEx.h"
#include "LayoutCodex.h"

Mesh::Mesh( Graphics& gfx, const Material& mat, const aiMesh& mesh ) noexcept(!IS_DEBUG) : Drawable( gfx, mat, mesh ) { }

void Mesh::Submit( FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform ) const noexcept(!IS_DEBUG)
{
	DirectX::XMStoreFloat4x4( &transform, accumulatedTransform );
	Drawable::Submit( frame );
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4( &transform );
}
#include "TransformCbufScaling.h"
#include "TechniqueProbe.h"

namespace Bind
{
	TransformCbufScaling::TransformCbufScaling( Graphics& gfx, float scale ) :
		TransformCbuf( gfx ), buffer( MakeLayout() )
	{
		buffer["scale"] = scale;
	}

	void TransformCbufScaling::Accept( TechniqueProbe& probe )
	{
		probe.VisitBuffer( buffer );
	}

	void TransformCbufScaling::Bind( Graphics& gfx ) noexcept
	{
		const float scale = buffer["scale"];
		const auto scaleMatrix = DirectX::XMMatrixScaling( scale,scale,scale );
		auto xf = GetTransforms( gfx );
		xf.modelView = xf.modelView * scaleMatrix;
		xf.modelViewProj = xf.modelViewProj * scaleMatrix;
		UpdateBind( gfx,xf );
	}

	std::unique_ptr<CloningBindable> TransformCbufScaling::Clone() const noexcept
	{
		return std::make_unique<TransformCbufScaling>( *this );
	}

	Dcb::RawLayout TransformCbufScaling::MakeLayout()
	{
		Dcb::RawLayout layout;
		layout.Add<Dcb::Float>( "scale" );
		return layout;
	}
}
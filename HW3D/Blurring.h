#pragma once
#include "BindableCommon.h"
#include "Math.h"

class Blurring
{
public:
	Blurring( Graphics& gfx, int radius = 7, float sigma = 2.6f )
		: shader( gfx, "BlurPS.cso" ), kcb( gfx, 0u ), ccb( gfx, 1u )
	{
		SetKernel( gfx, radius, sigma );
	}
	void Bind( Graphics& gfx ) noexcept
	{
		shader.Bind( gfx );
		kcb.Bind( gfx );
		ccb.Bind( gfx );
	}
	void SetHorizontal( Graphics& gfx )
	{
		ccb.Update( gfx, { TRUE } );
	}
	void SetVertical( Graphics& gfx )
	{
		ccb.Update( gfx, { FALSE } );
	}
	void SetKernel( Graphics& gfx, int radius, float sigma )
	{
		assert( radius <= 7 );
		Kernel k;
		k.nTaps = radius * 2 + 1;
		float sum = 0.0f;
		for ( int i = 0; i < k.nTaps; i++ )
		{
			const auto x = float( i - ( radius + 1 ) );
			const auto g = gauss( x, sigma );
			sum += g;
			k.coefficients[i].x = g;
		}
		for ( int i = 0; i < k.nTaps; i++ )
		{
			k.coefficients[i].x /= sum;
		}
		kcb.Update( gfx, k );
	}
private:
	struct Kernel
	{
		int nTaps;
		float padding[3];
		DirectX::XMFLOAT4 coefficients[15];
	};
	struct Control
	{
		BOOL horizontal;
		float padding[3];
	};
	Bind::PixelShader shader;
	Bind::PixelConstantBuffer<Kernel> kcb;
	Bind::PixelConstantBuffer<Control> ccb;
};
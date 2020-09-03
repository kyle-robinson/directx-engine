#pragma once
#include "BindableCommon.h"
#include "Math.h"
#include <imgui/imgui.h>

class Blurring
{
public:
	Blurring( Graphics& gfx, int radius = 7, float sigma = 2.6f )
		: shader( gfx, "BlurPS.cso" ), kcb( gfx, 0u ), ccb( gfx, 1u )
	{
		SetKernelGauss( gfx, radius, sigma );
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
	void ShowWindow( Graphics& gfx )
	{
		if( ImGui::Begin( "Blur", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
		{
			bool filterChanged = false;
			{
				const char* items[] = { "Gauss", "Box" };
				static const char* curItem = items[0];
				if ( ImGui::BeginCombo( "Filter Type", curItem ) )
				{
					for ( int n = 0; n < std::size( items ); n++ )
					{
						const bool isSelected = ( curItem == items[n] );
						if ( ImGui::Selectable( items[n], isSelected ) )
						{
							filterChanged = true;
							curItem = items[n];
							if ( curItem == items[0] )
							{
								kernelType = KernelType::Gauss;
							}
							else if ( curItem == items[1] )
							{
								kernelType = KernelType::Box;
							}
						}
						if ( isSelected )
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			bool radChange = ImGui::SliderInt( "Radius", &radius, 0, 15 );
			bool sigChange = ImGui::SliderFloat( "Sigma", &sigma, 0.1f, 10.0f );
			if ( radChange || sigChange || filterChanged )
			{
				if ( kernelType == KernelType::Gauss )
				{
					SetKernelGauss( gfx, radius, sigma );
				}
				else if ( kernelType == KernelType::Box )
				{
					SetKernelBox( gfx, radius );
				}
			}
		}
		ImGui::End();
	}
	void SetKernelGauss( Graphics& gfx, int radius, float sigma ) noexcept(!IS_DEBUG)
	{
		assert( radius <= maxRadius );
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
	void SetKernelBox( Graphics& gfx, int radius ) noexcept(!IS_DEBUG)
	{
		assert( radius <= maxRadius );
		Kernel k;
		k.nTaps = radius * 2 + 1;
		const float c = 1.0f / k.nTaps;
		for ( int i = 0; i < k.nTaps; i++ )
		{
			k.coefficients[i].x = c;
		}
		kcb.Update( gfx, k );
	}
private:
	enum class KernelType
	{
		Gauss,
		Box
	};
	static constexpr int maxRadius = 15;
	int radius;
	float sigma;
	KernelType kernelType = KernelType::Gauss;
	struct Kernel
	{
		int nTaps;
		float padding[3];
		DirectX::XMFLOAT4 coefficients[maxRadius * 2 + 1];
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
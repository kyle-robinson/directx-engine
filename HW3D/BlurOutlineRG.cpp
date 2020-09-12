#include "BlurOutlineRG.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineDrawPass.h"
#include "OutlineMaskPass.h"
#include "WireframePass.h"
#include "HorizontalBlurPass.h"
#include "VerticalBlurPass.h"
#include "BlurOutlineDrawPass.h"
#include "ShadowMappingPass.h"
#include "RenderTarget.h"
#include "DynamicConstant.h"
#include "ShadowSampler.h"
#include "ShadowRasterizer.h"
#include "SkyboxPass.h"
#include "Source.h"
#include "Math.h"
#include "imgui/imgui.h"

namespace Rgph
{
	BlurOutlineRG::BlurOutlineRG(Graphics& gfx) :
		RenderGraph(gfx)
	{
		{
			auto pass = std::make_unique<BufferClearPass>("clearRT");
			pass->SetSinkLinkage("buffer", "$.backbuffer");
			AppendPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<BufferClearPass>("clearDS");
			pass->SetSinkLinkage("buffer", "$.masterDepth");
			AppendPass(std::move(pass));
		}

		// shadow rasterizer
		{
			shadowRasterizer = std::make_shared<Bind::ShadowRasterizer>( gfx, 10000, 0.0005f, 1.0f );
			AddGlobalSource( DirectBindableSource<Bind::ShadowRasterizer>::Make( "shadowRasterizer", shadowRasterizer ) );
		}

		{
			auto pass = std::make_unique<ShadowMappingPass>( gfx, "shadowMap" );
			pass->SetSinkLinkage( "shadowRasterizer", "$.shadowRasterizer" );
			AppendPass( std::move( pass ) );
		}

		// shadow control buffer and sampler
		{
			{
				Dcb::RawLayout layout;
				layout.Add<Dcb::Integer>( "pcfLevel" );
				layout.Add<Dcb::Float>( "depthBias" );
				layout.Add<Dcb::Bool>( "hwPcf" );
				Dcb::Buffer buffer{ std::move( layout ) };
				buffer["pcfLevel"] = 0;
				buffer["depthBias"] = 0.0005f;
				buffer["hwPcf"] = true;
				shadowControl = std::make_shared<Bind::CachingPixelConstantBufferEx>( gfx, buffer, 2u );
				AddGlobalSource( DirectBindableSource<Bind::CachingPixelConstantBufferEx>::Make( "shadowControl", shadowControl ) );
			}
			{
				shadowSampler = std::make_shared<Bind::ShadowSampler>( gfx );
				AddGlobalSource( DirectBindableSource<Bind::ShadowSampler>::Make( "shadowSampler", shadowSampler ) );
			}
		}

		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetSinkLinkage("shadowMap", "shadowMap.map");
			pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
			pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
			pass->SetSinkLinkage("shadowControl", "$.shadowControl");
			pass->SetSinkLinkage("shadowSampler", "$.shadowSampler");
			AppendPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<SkyboxPass>( gfx, "skybox" );
			pass->SetSinkLinkage( "renderTarget", "lambertian.renderTarget" );
			pass->SetSinkLinkage( "depthStencil", "lambertian.depthStencil" );
			AppendPass( std::move( pass ) );
		}

		{
			auto pass = std::make_unique<OutlineMaskPass>(gfx, "outlineMask");
			pass->SetSinkLinkage("depthStencil", "skybox.depthStencil");
			AppendPass(std::move(pass));
		}

		// setup blur constant buffers
		{
			{
				Dcb::RawLayout layout;
				layout.Add<Dcb::Integer>("nTaps");
				layout.Add<Dcb::Array>("coefficients");
				layout["coefficients"].Set<Dcb::Float>(maxRadius * 2 + 1);
				Dcb::Buffer buf{ std::move(layout) };
				blurKernel = std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 0);
				SetKernelGauss(radius, sigma);
				AddGlobalSource(DirectBindableSource<Bind::CachingPixelConstantBufferEx>::Make("blurKernel", blurKernel));
			}
			{
				Dcb::RawLayout layout;
				layout.Add<Dcb::Bool>("isHorizontal");
				Dcb::Buffer buf{ std::move(layout) };
				blurDirection = std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1);
				AddGlobalSource(DirectBindableSource<Bind::CachingPixelConstantBufferEx>::Make("blurDirection", blurDirection));
			}
		}

		{
			auto pass = std::make_unique<BlurOutlineDrawPass>(gfx, "outlineDraw", gfx.GetWidth(), gfx.GetHeight());
			AppendPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<HorizontalBlurPass>("horizontal", gfx, gfx.GetWidth(), gfx.GetHeight());
			pass->SetSinkLinkage("scratchIn", "outlineDraw.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<VerticalBlurPass>("vertical", gfx);
			pass->SetSinkLinkage("renderTarget", "skybox.renderTarget");
			pass->SetSinkLinkage("depthStencil", "outlineMask.depthStencil");
			pass->SetSinkLinkage("scratchIn", "horizontal.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<WireframePass>( gfx, "wireframe" );
			pass->SetSinkLinkage( "renderTarget", "vertical.renderTarget" );
			pass->SetSinkLinkage( "depthStencil", "vertical.depthStencil" );
			AppendPass( std::move( pass ) );
		}
		SetSinkTarget("backbuffer", "wireframe.renderTarget");

		Finalize();
	}

	void BlurOutlineRG::RenderWindows( Graphics& gfx )
	{
		RenderKernelWindow( gfx );
		RenderShadowWindow( gfx );
	}

	void BlurOutlineRG::RenderKernelWindow( Graphics& gfx )
	{
		if ( ImGui::Begin( "Kernel", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
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
								kernelType = KernelType::Gauss;
							else if ( curItem == items[1] )
								kernelType = KernelType::Box;

							if ( isSelected )
								ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				bool radChange = ImGui::SliderInt( "Radius", &radius, 0, maxRadius );
				bool sigChange = ImGui::SliderFloat( "Sigma", &sigma, 0.1f, 10.0f );
				if ( radChange || sigChange || filterChanged )
				{
					if ( kernelType == KernelType::Gauss )
						SetKernelGauss( radius, sigma );
					else if ( kernelType == KernelType::Box )
						SetKernelBox( radius );
				}
			}
		}
		ImGui::End();
	}

	void BlurOutlineRG::RenderShadowWindow( Graphics& gfx )
	{
		if ( ImGui::Begin( "Shadows", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
		{
			auto shadowBuf = shadowControl->GetBuffer();
			bool bilin = shadowSampler->GetBilinear();

			bool pcfChange = ImGui::SliderInt( "PCF Level", &shadowBuf["pcfLevel"], 0, 4 );
			bool biasChange = ImGui::SliderFloat( "Depth Bias / Pre", &shadowBuf["depthBias"], 0.0f, 0.1f, "%.6f", 3.6f );
			bool hwPcfChange = ImGui::Checkbox( "HW PCF", &shadowBuf["hwPcf"] ); ImGui::SameLine();
			ImGui::Checkbox( "Bilinear", &bilin );
			
			if ( pcfChange || biasChange || hwPcfChange )
				shadowControl->SetBuffer( shadowBuf );

			shadowSampler->SetHwPcf( shadowBuf["hwPcf"] );
			shadowSampler->SetBilinear( bilin );

			{
				auto bias = shadowRasterizer->GetDepthBias();
				auto slope = shadowRasterizer->GetSlopeBias();
				auto clamp = shadowRasterizer->GetClamp();

				bool biasChange = ImGui::SliderInt( "Depth Bias / Post", &bias, 0, 100000 );
				bool slopeChange = ImGui::SliderFloat( "Slope Bias", &slope, 0.0f, 100.0f, "%.4f", 4.0f );
				bool clampChange = ImGui::SliderFloat( "Clamp", &clamp, 0.0001f, 0.5f, "%.4f", 2.5f );

				if ( biasChange || slopeChange || clampChange )
					shadowRasterizer->ChangeDepthBiasParameters( gfx, bias, slope, clamp );
			}
		}
		ImGui::End();
	}

	void BlurOutlineRG::SetKernelGauss( int radius, float sigma ) noexcept(!IS_DEBUG)
	{
		assert( radius <= maxRadius );
		auto kernel = blurKernel->GetBuffer();
		const int nTaps = radius * 2 + 1;
		kernel["nTaps"] = nTaps;
		float sum = 0.0f;
		
		for ( int i = 0; i < nTaps; i++ )
		{
			const auto x = float(i - radius);
			const auto g = gauss(x, sigma);
			sum += g;
			kernel["coefficients"][i] = g;
		}
		
		for ( int i = 0; i < nTaps; i++ )
			kernel["coefficients"][i] = (float)kernel["coefficients"][i] / sum;

		blurKernel->SetBuffer( kernel );
	}

	void BlurOutlineRG::SetKernelBox( int radius ) noexcept(!IS_DEBUG)
	{
		assert( radius <= maxRadius );
		auto kernel = blurKernel->GetBuffer();
		const int nTaps = radius * 2 + 1;
		kernel["nTaps"] = nTaps;
		const float coefficients = 1.0f / nTaps;
		
		for ( int i = 0; i < nTaps; i++ )
			kernel["coefficients"][i] = coefficients;

		blurKernel->SetBuffer( kernel );
	}

	void BlurOutlineRG::DumpShadowMap( Graphics& gfx, const std::string& path )
	{
		dynamic_cast<ShadowMappingPass&>( FindPassByName( "shadowMap" ) ).DumpShadowMap( gfx, path );
	}

	void BlurOutlineRG::BindMainCamera( Camera& cam )
	{
		dynamic_cast<LambertianPass&>( FindPassByName( "lambertian" ) ).BindMainCamera( cam );
		dynamic_cast<SkyboxPass&>( FindPassByName( "skybox" ) ).BindMainCamera( cam );
	}

	void BlurOutlineRG::BindShadowCamera( Camera& cam )
	{
		dynamic_cast<ShadowMappingPass&>( FindPassByName( "shadowMap" ) ).BindShadowCamera( cam );
		dynamic_cast<LambertianPass&>( FindPassByName( "lambertian" ) ).BindShadowCamera( cam );
	}
}
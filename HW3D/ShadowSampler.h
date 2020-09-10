#pragma once
#include "Bindable.h"

namespace Bind
{
	class ShadowSampler : public Bindable
	{
	public:
		ShadowSampler( Graphics& gfx );
		void Bind( Graphics& gfx ) noexcept(!IS_DEBUG) override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	};
}
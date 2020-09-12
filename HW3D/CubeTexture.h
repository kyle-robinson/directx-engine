#pragma once
#include "Bindable.h"

class Surface;

namespace Bind
{
	class CubeTexture : public Bindable
	{
	public:
		CubeTexture( Graphics& gfx, const std::string& path, UINT slot = 0u );
		void Bind( Graphics& gfx ) noexcept(!IS_DEBUG) override;
	private:
		unsigned int slot;
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};
}
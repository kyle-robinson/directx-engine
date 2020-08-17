#pragma once
#include "Bindable.h"

namespace Bind
{
	class PixelShader : public Bindable
	{
	public:
		PixelShader( Graphics& gfx, const std::wstring& path );
		void Bind( Graphics& gfx ) noexcept override;
		ID3DBlob* GetByteCode() const noexcept;
	protected:
		Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	};
}
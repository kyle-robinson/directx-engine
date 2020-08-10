#pragma once
#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader( Graphics& gfx, const std::wstring& path );
	void Bind( Graphics& gfx ) noexcept override;
	ID3DBlob* GetByteCode() const noexcept;
	std::wstring ToWide( const std::string& narrow );
protected:
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};
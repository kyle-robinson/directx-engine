#pragma once
#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader( Graphics& gfx, const std::wstring& path );
	void Bind( Graphics& gfx ) noexcept override;
	ID3DBlob* GetByteCode() const noexcept;
	std::wstring ToWide( const std::string& narrow );
protected:
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
};
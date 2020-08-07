#pragma once
#include "WindowsInclude.h"
#include <vector>
#include <string>
#include <wrl.h>
#include <dxgidebug.h>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	DxgiInfoManager( const DxgiInfoManager& ) = delete;
	DxgiInfoManager& operator = ( const DxgiInfoManager& ) = delete;
	~DxgiInfoManager() = default;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};
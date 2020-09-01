#include "Bindable.h"

namespace Bind
{
	ID3D11DeviceContext* Bindable::GetContext( Graphics& gfx ) noexcept
	{
		return gfx.pContext.Get();
	}

	ID3D11Device* Bindable::GetDevice( Graphics& gfx ) noexcept
	{
		return gfx.pDevice.Get();
	}

	DxgiInfoManager& Bindable::GetInfoManager( Graphics& gfx )
	{
	#ifndef N_DEBUG
		return gfx.infoManager;
	#else
		throw std::logic_error( "ERROR:: Tried to access gfx.infoManager in Release config!" );
	#endif
	}
}
#pragma once
#include "Graphics.h"

namespace Bind
{
	class Bindable
	{
	public:
		virtual void Bind( Graphics& gfx ) noexcept = 0;
		virtual ~Bindable() = default;
		virtual std::string GetUID() const noexcept
		{
			assert( false );
			return "";
		}
	protected:
		static ID3D11DeviceContext* GetContext( Graphics& gfx ) noexcept;
		static ID3D11Device* GetDevice( Graphics& gfx ) noexcept;
		static DxgiInfoManager& GetInfoManager( Graphics& gfx ) noexcept(!IS_DEBUG);
	};
}
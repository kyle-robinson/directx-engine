#pragma once
#include "DynamicConstant.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace Dcb
{
	class LayoutCodex
	{
	public:
		static CompleteLayout Resolve( RawLayout&& layout ) noexcept(!IS_DEBUG);
	private:
		static LayoutCodex& Get_() noexcept;
		std::unordered_map<std::string, std::shared_ptr<LayoutElement>> map;
	};
}
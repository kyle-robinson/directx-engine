#pragma once
#include "DynamicConstant.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace DCB
{
	class LayoutCodex
	{
	public:
		static DCB::CompleteLayout Resolve( DCB::RawLayout&& layout ) noexcept(!IS_DEBUG);
	private:
		static LayoutCodex& Get_() noexcept;
		std::unordered_map<std::string, std::shared_ptr<DCB::LayoutElement>> map;
	};
}
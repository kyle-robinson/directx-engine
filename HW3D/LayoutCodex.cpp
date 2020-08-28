#include "LayoutCodex.h"

namespace DCB
{
	DCB::CompleteLayout LayoutCodex::Resolve( DCB::RawLayout&& layout ) noexcept(!IS_DEBUG)
	{
		auto sig = layout.GetSignature();
		auto& map = Get_().map;
		const auto i = map.find( sig );

		// identical layout already exists
		if ( i != map.end() )
		{
			layout.ClearRoot();
			return { i->second };
		}

		// otherwise add layout root element to map
		auto result = map.insert( { std::move( sig ), layout.DeliverRoot() } );
		// return layout with extra ref to root
		return { result.first->second };
	}

	LayoutCodex& LayoutCodex::Get_() noexcept
	{
		static LayoutCodex codex;
		return codex;
	}
}
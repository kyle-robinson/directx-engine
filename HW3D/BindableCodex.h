#pragma once
#include "Bindable.h"
#include "BindableCodex.h"
#include <memory>
#include <unordered_map>

namespace Bind
{
	class Codex
	{
	public:
		template<class T, typename...Params>
		static std::shared_ptr<Bindable> Resolve( Graphics& gfx, Params&&...p ) noexcept(IS_DEBUG)
		{
			return Get().Resolve_<T>( gfx, std::forward<Params>( p )... );
		}
	private:
		template<class T, typename...Params>
		std::shared_ptr<Bindable> Resolve_( Graphics& gfx, Params&&...p ) noexcept(!IS_DEBUG)
		{
			const auto key = T::GenerateUID( std::forward<Params>( p )... );
			const auto i = binds.find( key );
			if ( i == binds.end() )
			{
				auto bind = std::make_shared<T>( gfx, std::forward<Params>( p )... );
				binds[key] = bind;
				return bind;
			}
			else
			{
				return i->second;
			}
		}
		static Codex& Get()
		{
			static Codex codex;
			return codex;
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<Bindable>> binds;
	};
}
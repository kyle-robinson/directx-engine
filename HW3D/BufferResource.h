#pragma once

class Graphics;

namespace Bind
{
	class BufferResource
	{
	public:
		virtual ~BufferResource() = default;
		virtual void BindAsBuffer( Graphics& ) noexcept(!IS_DEBUG) = 0;
		virtual void BindAsBuffer( Graphics&, BufferResource* ) noexcept(!IS_DEBUG) = 0;
		virtual void Clear( Graphics& ) noexcept(!IS_DEBUG) = 0;
	};
}
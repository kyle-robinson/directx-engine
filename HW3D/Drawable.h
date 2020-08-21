#pragma once
#include "Graphics.h"
#include <memory>

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable( const Drawable& ) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw( Graphics& gfx ) const noexcept(!IS_DEBUG);
	virtual ~Drawable() = default;
protected:
	void AddBind( std::shared_ptr<Bind::Bindable> bind ) noexcept(!IS_DEBUG);
private:
	const Bind::IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bind::Bindable>> binds;
};
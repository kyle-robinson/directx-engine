#pragma once

class Drawable;
class Graphics;
class Step;

namespace Rgph
{
	class Job
	{
	public:
		Job(const Step* pStep, const Drawable* pDrawable);
		void Execute(Graphics& gfx) const noexcept(!IS_DEBUG);
	private:
		const class Drawable* pDrawable;
		const class Step* pStep;
	};
}
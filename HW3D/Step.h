#pragma once
#include "Bindable.h"
#include "Graphics.h"
#include <vector>
#include <memory>

class TechniqueProbe;
class Drawable;

class Step
{
public:
	Step( size_t targetPass_in );
	Step( Step&& ) = default;
	Step( const Step& src ) noexcept : targetPass( src.targetPass );
	Step& operator=( const Step& ) = delete;
	Step& operator=( Step&& ) = delete;
	void AddBindable( std::shared_ptr<Bind::Bindable> bind_in ) noexcept;
	void Submit( class FrameCommander& grame, const class Drawable& drawable ) const;
	void Bind( Graphics& gfx ) const;
	void InitializeParentReferences( const class Drawable& parent ) noexcept;
	void Accept( TechniqueProbe& probe );
private:
	size_t targetPass;
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
};
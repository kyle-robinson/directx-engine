#pragma once
#include "TechniqueProbe.h"
#include "Bindable.h"
#include "Graphics.h"
#include <vector>
#include <memory>

class Step
{
public:
	Step( size_t targetPass_in ) : targetPass{ targetPass_in } {}
	void AddBindable( std::shared_ptr<Bind::Bindable> bind_in ) noexcept
	{
		bindables.push_back( std::move( bind_in ) );
	}
	void Submint( class FrameCommander& grame, const class Drawable& drawable ) const;
	void Bind( Graphics& gfx ) const
	{
		for ( const auto& b : bindables )
		{
			b->Bind( gfx );
		}
	}
	void InitializeParentReferences( const class Drawable& parent ) noexcept;
	void Accept( TechniqueProbe& probe )
	{
		probe.SetStep( this );
		for ( auto& pb : bindables )
		{
			pb->Accept( probe );
		}
	}
private:
	size_t targetPass;
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
};
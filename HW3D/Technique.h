#pragma once
#include "Step.h"
#include <vector>

class TechniqueProbe;
class Drawable;
class RenderGraph;

class Technique
{
public:
	Technique() = default;
	Technique( std::string name, bool startActive = true ) noexcept;
	void Submit( const Drawable& drawable ) const noexcept;
	void AddStep( Step step ) noexcept;
	bool IsActive() const noexcept;
	void SetActiveState( bool active_in ) noexcept;
	void InitializeParentReference( const Drawable& parent ) noexcept;
	void Accept( TechniqueProbe& probe );
	const std::string& GetName() const noexcept;
	void Link( RenderGraph& rg );
private:
	bool active = true;
	std::vector<Step> steps;
	std::string name;
};
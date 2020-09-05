#pragma once
#include "Bindable.h"
#include "Graphics.h"
#include <vector>
#include <memory>

class TechniqueProbe;
class Drawable;
class RenderQueuePass;
class RenderGraph;

class Step
{
public:
	Step( std::string targetPassName );
	Step( Step&& ) = default;
	Step( const Step& src ) noexcept;
	Step& operator=( const Step& ) = delete;
	Step& operator=( Step&& ) = delete;
	void AddBindable( std::shared_ptr<Bind::Bindable> bind_in ) noexcept;
	void Submit( const class Drawable& drawable ) const;
	void Bind( Graphics& gfx ) const;
	void InitializeParentReferences( const class Drawable& parent ) noexcept;
	void Accept( TechniqueProbe& probe );
	void Link( RenderGraph& rg );
private:
	std::string targetPassName;
	RenderQueuePass* pTargetPass = nullptr;
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
};
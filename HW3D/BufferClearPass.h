#pragma once
#include "Pass.h"

class BufferClearPass : public Pass
{
public:
	BufferClearPass( std::string name );
	void Execute( Graphics& gfx ) const noexcept(!IS_DEBUG) override;
};
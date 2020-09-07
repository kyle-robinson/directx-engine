#pragma once
#include <limits>

namespace Dcb
{
	class Buffer;
}
class Technique;
class Step;

class TechniqueProbe
{
public:
	virtual ~TechniqueProbe();
	void SetTechnique( Technique* pTech_in )
	{
		pTech = pTech_in;
		techIdx++;
		OnSetTechnique();
	}
	void SetStep( Step* pStep_in )
	{
		pStep = pStep_in;
		stepIdx++;
		OnSetStep();
	}
	bool VisitBuffer( Dcb::Buffer& buf )
	{
		bufIdx++;
		return OnVisitBuffer( buf );
	}
protected:
	virtual void OnSetStep() {}
	virtual void OnSetTechnique() {}
	virtual bool OnVisitBuffer( Dcb::Buffer& )
	{
		return false;
	}
protected:
	Step* pStep = nullptr;
	Technique* pTech = nullptr;
	size_t bufIdx = std::numeric_limits<size_t>::max();
	size_t techIdx = std::numeric_limits<size_t>::max();
	size_t stepIdx = std::numeric_limits<size_t>::max();
};
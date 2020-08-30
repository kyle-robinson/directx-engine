#pragma once

namespace Dcb
{
	class Buffer;
}

class TechniqueProbe
{
public:
	void SetTechnique( class Technique* pTech_in )
	{
		pTech = pTech_in;
		techIdx++;
		OnSetTechnique();
	}
	void SetStep( class Step* pStep_in )
	{
		pStep = pStep_in;
		stepIdx++;
		OnSetStep();
	}
	bool VisitBuffer( class Dcb::Buffer& buf )
	{
		bufIdx++;
		return OnVisitBuffer( buf );
	}
protected:
	virtual void OnSetStep() {}
	virtual void OnSetTechnique() {}
	virtual bool OnVisitBuffer( class Dcb::Buffer& ) = 0;
protected:
	class Step* pStep = nullptr;
	class Technique* pTech = nullptr;
	size_t bufIdx = std::numeric_limits<size_t>::max();
	size_t techIdx = std::numeric_limits<size_t>::max();
	size_t stepIdx = std::numeric_limits<size_t>::max();
};
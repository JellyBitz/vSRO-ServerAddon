#pragma once
#include "CGObj.h"

// Game object with mobile behavior
class CGObjMobile : public CGObj
{
public: /// Public Methods
	// Adds a buff for determinated duration which cannot exceed the skill duration
	void AddBuff(uint32_t SkillId, uint32_t Seconds);
};
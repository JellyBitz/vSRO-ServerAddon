#pragma once
#include "CGObjMobile.h"

// Game object for a interactive character 
class CGObjChar : public CGObjMobile
{
public:
	// Drops an item near the current position
	void DropItem(const char* Codename, uint32_t Amount, uint8_t OptLevel);
};
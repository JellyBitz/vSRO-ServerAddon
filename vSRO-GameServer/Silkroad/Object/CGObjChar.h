#pragma once
#include "CGObjMobile.h"

// Game object for a interactive character 
class CGObjChar : public CGObjMobile
{
public: /// Public Properties
	// Sets the body mode
	void SetBodyMode(uint8_t Value);
public: /// Public Methods
	// Drops an item near the current position
	void DropItem(const char* Codename, uint32_t Amount, uint8_t OptLevel);
};
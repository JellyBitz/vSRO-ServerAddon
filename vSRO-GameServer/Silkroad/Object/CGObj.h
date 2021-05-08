#pragma once
#include <cstdint>

// Base class for any kind object in game
class CGObj
{
protected: /// Protected Members
	uint32_t ptrVTable;
public: /// Public Properties
	// Gets the game world id as reference
	void GetGameWorldId(uint32_t* OutGameWorldId);
	// Set health status 
	void SetLifeState(uint8_t State);
};
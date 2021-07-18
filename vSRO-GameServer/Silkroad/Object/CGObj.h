#pragma once
#include <cstdint>

// Base class for any kind object in game
class CGObj
{
protected: /// Protected Members
	uint32_t m_ptrVTable;
	// Unique Identifier from this object
	uint32_t m_UniqueID;
public: /// Public Properties
	// Gets the game world id as reference
	void GetGameWorldId(uint32_t* OutGameWorldId);
public: // Public Methods 
	// Set life state
	void SetLifeState(bool Alive);
private: /// Private Helpers
	// Set life state
	void SetLifeState(uint8_t State, int32_t Unknown01, float Unknown02);
};
#pragma once
#include "CGObjChar.h"

// Game map position
class CPosition
{
public:
	uint16_t RegionID;
	uint16_t UnkUShort01;
	uint32_t PosX;
	uint32_t PosY;
	uint32_t PosZ;
};

// Game object for a player character
class CGObjPC : public CGObjChar
{
private: /// Private Members
	char pad_0x4[128];
	// Map position
	CPosition m_Position;
public: /// Public Methods
	// Add item to inventory. Return error code
	uint16_t AddItem(const char* Codename, int32_t Count, bool RandomizeStats, int8_t OptLevel);
	// Update the gold this player has
	void UpdateGold(int64_t Offset);
	// Update hwan title by level
	void UpdateHwan(uint8_t Level);
	// Moves the player to the map location. Return success
	bool MoveTo(uint16_t RegionId, uint32_t PosX, uint32_t PosY, uint32_t PosZ);
	// Moves the player to the gameworld and map location. Return success
	bool MoveTo(uint32_t GameWorldId, uint16_t RegionId, uint32_t PosX, uint32_t PosY, uint32_t PosZ);
	// Transform an item to another one from inventory slot
	bool MutateItemAt(uint8_t Slot,const char* NewCodename);
private: /// Private Helpers
	// Update the gold amount
	void UpdateGold(int64_t Amount, int32_t Unknown, bool Realtime, bool ShowMessage);
	// Moves the player to the location specified
	bool MoveTo(const uint32_t GameWorldId, CPosition Position, int32_t Type);
};
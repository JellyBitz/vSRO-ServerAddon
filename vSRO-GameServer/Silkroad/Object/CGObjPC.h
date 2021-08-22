#pragma once
#include "CGObjChar.h"
#include "../Navigation/CPosition.h"
#include "CInstancePC.h"

// Game object for a player character
class CGObjPC : public CGObjChar
{
private: /// Private Members
	char pad_0x8[44];
	CInstancePC* m_CInstancePC;
	char pad_0x38[76];
	CPosition m_Position;
public: /// Public Properties
	// Gets the CharId from database
	uint32_t GetCharID();
	// Gets the current position
	CPosition GetPosition();
public: /// Public Methods
	// Add item to inventory. Return error code
	uint16_t AddItem(const char* Codename, int32_t Count, bool RandomizeStats, int8_t OptLevel);
	// Update the gold this player has
	void UpdateGold(int64_t Offset);
	// Update hwan title by level
	void UpdateHwan(uint8_t Level);
	// Update the current Skill Points
	void UpdateSP(int32_t Offset);
	// Update level experience
	void UpdateExperience(int64_t ExpOffset);
	// Add skill experience
	void AddSPExperience(uint32_t SPExpOffset);
	// Reduces health and/or mana points. If health reduced exceeds the current amount, the player will die
	void ReduceHPMP(uint32_t Health, uint32_t Mana, bool ShowEffect);
	// Updates the cape state from PVP
	void UpdatePVPCapeType(uint8_t CapeType);
	// Moves the player to the map location. Return success
	bool MoveTo(uint16_t RegionId, float PosX, float PosY, float PosZ);
	// Moves the player to the gameworld and map location. Return success
	bool MoveTo(uint32_t GameWorldId, uint16_t RegionId, float PosX, float PosY, float PosZ);
	// Transform an item to another one from inventory slot
	bool MutateItemAt(uint8_t Slot,const char* NewCodename);
	// Moves the player to the same position to force a reloading
	bool Reload();
	// Set the guild member nickname
	void ApplyGuildNickName(const char* Nickname);
private: /// Private Helpers
	// Update the gold amount
	void UpdateGold(int64_t Amount, int32_t Unknown, bool Realtime, bool ShowMessage);
	// Moves the player to the location specified
	bool MoveTo(const uint32_t GameWorldId, CPosition Position, int32_t Type);
};
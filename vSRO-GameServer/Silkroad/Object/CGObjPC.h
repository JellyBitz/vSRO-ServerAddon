#pragma once
#include <cstdint>

#include "CGObjChar.h"
// Game object for a player character
class CGObjPC : public CGObjChar
{
public: /// Public Methods
	// Add item to inventory. Return error code
	uint16_t AddItem(const char* Codename, int32_t Count, int32_t Variance, int8_t OptLevel);
	// Update the gold this player has
	void UpdateGold(int64_t Offset);
	// Update hwan title by level
	void UpdateTitle(uint8_t Level);
private: /// Private Helpers
	// Update the gold amount
	void UpdateGold(int64_t Amount, int32_t Unknown, bool Realtime, bool ShowMessage);
};
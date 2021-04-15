#pragma once
#include <cstdint>

// Game map position
class CPosition
{
public: // Public Properties
	uint16_t RegionId;
	uint16_t UnkUShort01;
	float PosX;
	float PosY;
	float PosZ;
public: /// Constructors
	CPosition();
	CPosition(uint16_t RegionId, uint16_t UnkUShort01, float PosX, float PosY, float PosZ);
};
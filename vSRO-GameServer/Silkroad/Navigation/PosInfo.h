#pragma once
#include <cstdint>

struct PosInfo
{
	uint16_t region_id;
	float x;
	float y;
	float z;

	PosInfo() : region_id(0), x(0), y(0), z(0)
	{
	}
	PosInfo(uint16_t region_id, float x, float y, float z) : region_id(region_id), x(x), y(y), z(z)
	{
	}
};
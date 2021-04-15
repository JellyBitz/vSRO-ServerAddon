#pragma once
#include "PosInfo.h"

struct NavInfo
{
	std::uintptr_t* crt_nav_cell_quad_ptr;
	int32_t unk0;
	int16_t region_id;
	int16_t unk1;
	float x;
	float y;
	float z;

	NavInfo() : crt_nav_cell_quad_ptr(nullptr), unk0(0), region_id(0), unk1(0), x(0), y(0), z(0)
	{
	}

	NavInfo(int16_t region_id, float x, float y, float z) : crt_nav_cell_quad_ptr(nullptr), unk0(0), region_id(region_id), unk1(0), x(x), y(y), z(z)
	{
	}

	NavInfo(const PosInfo& pos_info) : crt_nav_cell_quad_ptr(nullptr), unk0(0), region_id(pos_info.region_id), unk1(0), x(pos_info.x), y(pos_info.y), z(pos_info.z)
	{

	}
};
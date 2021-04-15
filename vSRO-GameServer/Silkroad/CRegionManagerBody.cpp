#include "CRegionManagerBody.h"

CRegionManagerBody* CRegionManagerBody::GetInstance()
{
	return *reinterpret_cast<CRegionManagerBody**>(0x400000 + 0x8C387C);
}

bool CRegionManagerBody::ResolveCellAndHeight(NavInfo* NavInfoPtr)
{
	return reinterpret_cast<bool(__thiscall*)(CRegionManagerBody*, NavInfo*, int)>(0x400000 + 0x58B1D0)(GetInstance(), NavInfoPtr, 0);
}
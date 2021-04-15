#pragma once
#include "Navigation/NavInfo.h"
class CRegionManagerBody
{
private:
	static CRegionManagerBody* GetInstance();
public:
	// Check if the navigation info provided can be solved for a successfully spawn
	static bool ResolveCellAndHeight(NavInfo* NavInfoPtr);
};
#pragma once
#include "Object/CGObjPC.h"
#include "Object/CGObjMob.h"

// Manager which controls all objects in game
class CGObjManager
{
public: /// Public Methods
	// Returns the object pointer from player by using his charname
	static CGObjPC* GetObjPCByCharName16(const char* CharName16);
	// Creates a mob in the position specified
	static CGObjMob* CreateMob(uint32_t RefObjId, uint32_t GameWorldId, uint16_t RegionId, float PosX, float PosY, float PosZ, float GenerateRadius);
	// Returns base data from object
	static void* GetRefObjBaseData(int RefObjId);
	// Gets the creep type from object
	static int32_t GetCreepType(void* RefObjBaseDataPtr);
	// Gets the behavior from object
	static void* GetRefTactic(void* RefObjBaseDataPtr);
};
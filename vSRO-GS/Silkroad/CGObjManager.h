#pragma once
#include "Object/CGObjPC.h"
// Manager which controls all objects in game
class CGObjManager
{
public: /// Public Methods
	// Returns the object pointer from player by using his charname
	static CGObjPC* GetObjPCByCharName16(const char* CharName16);
};
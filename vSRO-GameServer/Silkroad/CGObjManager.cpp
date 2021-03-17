#include "CGObjManager.h"

CGObjPC* CGObjManager::GetObjPCByCharName16(const char* CharName16)
{
	// 0x00400000 (gs base addr) + 0x85E20 (offset);
	return reinterpret_cast<CGObjPC * (__cdecl*)(const char*)>(0x00485E20)(CharName16);
}
#include "CGObjChar.h"
#include "../../Utils/Memory/hook.h"

void CGObjChar::DropItem(const char* Codename, uint32_t Amount, uint8_t OptLevel)
{
	CallVirtual<void(__thiscall*)(CGObjChar*, const char*, uint32_t, uint32_t, uint8_t*, uint32_t, int32_t, uint32_t, int32_t)>(this, 149)(this, Codename, Amount, 0, &OptLevel, 0, 1, 0, 0);
}
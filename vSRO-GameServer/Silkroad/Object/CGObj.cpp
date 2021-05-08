#include "CGObj.h"
#include "../../Utils/Memory/hook.h"

/* Public Properties */
void CGObj::GetGameWorldId(uint32_t* OutGameWorldId)
{
	CallVirtual<void(__thiscall*)(CGObj*, uint32_t*)>(this, 5)(this, OutGameWorldId);
}
void CGObj::SetLifeState(uint8_t State)
{
	CallVirtual<void(__thiscall*)(CGObj*, uint8_t, int, float)>(this, 124)(this, State, 255, 0);
}
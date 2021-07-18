#include "CGObj.h"
#include "../../Utils/Memory/hook.h"

/* Public Properties */
void CGObj::GetGameWorldId(uint32_t* OutGameWorldId)
{
	CallVirtual<void(__thiscall*)(CGObj*, uint32_t*)>(this, 5)(this, OutGameWorldId);
}
/* Public Methods */
void CGObj::SetLifeState(bool Alive)
{
	SetLifeState(Alive ? 1 : 2, 0, 0);
}
/* Private Helpers */
void CGObj::SetLifeState(uint8_t State, int32_t Unknown01, float Unknown02)
{
	CallVirtual<void(__thiscall*)(CGObj*, uint8_t, int32_t, float)>(this, 124)(this, State, Unknown01, Unknown02);
}
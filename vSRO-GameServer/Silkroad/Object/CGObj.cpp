#include "CGObj.h"
#include "../../Utils/Memory.h"

/* Public Properties */
void CGObj::GetGameWorldId(uint32_t* OutGameWorldId)
{
	call_virtual<void(__thiscall*)(CGObj*, uint32_t*)>(this, 5)(this, OutGameWorldId);
}
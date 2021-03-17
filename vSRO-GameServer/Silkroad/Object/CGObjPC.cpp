#include "CGObjPC.h"
#include "../../Utils/Memory.h"

uint16_t CGObjPC::AddItem(const char* Codename, int32_t Count, int32_t Variance, int8_t OptLevel)
{
	int16_t operation_result = 0;
	call_virtual<void(__thiscall*)(CGObjPC*, int32_t, const char*, int32_t, int16_t*, int8_t, int32_t, int32_t, int8_t, int32_t, int32_t, int8_t, int32_t)>(this, 139)(this, 0, Codename, Count, &operation_result, 0, Variance, 0, OptLevel, 0, 1, 0, 0);
	return operation_result;
}
void CGObjPC::UpdateGold(int64_t Offset, int32_t Unknown, bool Realtime, bool ShowMessage)
{
	call_virtual<void(__thiscall*)(CGObjPC*, int64_t, int32_t, int32_t, int32_t)>(this, 91)(this, Offset, Unknown, (Realtime?1:0), (ShowMessage?1:0));
}
void CGObjPC::UpdateGold(int64_t Offset)
{
	UpdateGold(Offset,0,true,true);
}
void CGObjPC::UpdateTitle(uint8_t Level)
{
	reinterpret_cast<void(__thiscall*)(CGObjPC*, uint8_t)>(0x004A9F40)(this, Level);
}
#include "CGObjPC.h"
#include "../../Utils/Memory/hook.h"
#include "../../Silkroad/Text/GString.h"

/* Public Properties */
uint32_t CGObjPC::GetUniqueID()
{
	return m_UniqueID;
}
uint32_t CGObjPC::GetCharID()
{
	return m_CInstancePC->CharID;
}
CPosition CGObjPC::GetPosition()
{
	return m_Position;
}
/* Public Methods */
uint16_t CGObjPC::AddItem(const char* Codename, int32_t Amount, bool RandomizeStats, int8_t OptLevel)
{
	int16_t operation_result = 0;
	CallVirtual<void(__thiscall*)(CGObjPC*, int32_t, const char*, int32_t, int16_t*, int8_t, int32_t, int32_t, int8_t, int32_t, int32_t, int8_t, int32_t)>(this, 139)(this, 0, Codename, Amount, &operation_result, 0, RandomizeStats ? 1 : 0, 0, OptLevel, 0, 1, 0, 0);
	return operation_result;
}
void CGObjPC::UpdateGold(int64_t Offset)
{
	// Check if offset is higher than int.MaxValue to avoid send a bugged message
	if(Offset > 0x7FFFFFFF)
		UpdateGold(Offset, 25, true, false);
	else
		UpdateGold(Offset, 25, true, true);
}
void CGObjPC::UpdateHwan(uint8_t Level)
{
	reinterpret_cast<void(__thiscall*)(CGObjPC*, uint8_t)>(0x004A9F40)(this, Level);
}
void CGObjPC::UpdateSP(int32_t Offset)
{
	CallVirtual<void(__thiscall*)(CGObjPC*, int32_t, int8_t)>(this, 93)(this, Offset, 1);
}
void CGObjPC::UpdateHPMP(int32_t Health, int32_t Mana, uint16_t DisplayEffectType)
{
	CallVirtual<void(__thiscall*)(CGObjPC*, int32_t, int32_t, uint16_t)>(this, 194)(this, Health, Mana, DisplayEffectType);
}
bool CGObjPC::MoveTo(uint16_t RegionId, float PosX, float PosY, float PosZ)
{
	uint32_t gwid = 0;
	GetGameWorldId(&gwid);
	CPosition pos(RegionId, m_Position.UnkUShort01,PosX,PosY,PosZ);
	return MoveTo(gwid, pos, 2);
}
bool CGObjPC::MoveTo(uint32_t GameWorldId, uint16_t RegionId, float PosX, float PosY, float PosZ)
{
	CPosition pos(RegionId, m_Position.UnkUShort01, PosX, PosY, PosZ);
	return MoveTo(GameWorldId, pos, 2);
}
bool CGObjPC::MutateItemAt(uint8_t Slot,const char* NewCodename)
{
	return CallVirtual<bool(__thiscall*)(CGObjPC*, int32_t, const char*)>(this, 141)(this, Slot, NewCodename);
}
bool CGObjPC::Reload()
{
	uint32_t gwid = 0;
	GetGameWorldId(&gwid);
	return MoveTo(gwid, m_Position, 2);
}
void CGObjPC::ApplyGuildNickName(const char* Nickname)
{
	std::n_string* nick = new std::n_string(Nickname);
	uint32_t charId = m_CInstancePC->CharID;

	__asm
	{
		push nick;
		push 0x25; // Update type
		push this;

		mov ecx, charId;
		xor edx, edx;
		
		mov eax, 0x005C80A0;
		call eax;
	}
}
/* Private Helpers */
void CGObjPC::UpdateGold(int64_t Offset, int32_t Unknown, bool Realtime, bool ShowMessage)
{
	CallVirtual<void(__thiscall*)(CGObjPC*, int64_t, int32_t, int32_t, int32_t)>(this, 91)(this, Offset, Unknown, (Realtime ? 1 : 0), (ShowMessage ? 1 : 0));
}
bool CGObjPC::MoveTo(uint32_t GameWorldId, CPosition Position, int32_t Type)
{
	return CallVirtual<bool(__thiscall*)(CGObjPC*, uint32_t*, CPosition, int32_t)>(this, 222)(this, &GameWorldId, Position, Type);
}
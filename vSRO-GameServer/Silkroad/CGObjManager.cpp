#include "CGObjManager.h"
#include "Navigation/NavInfo.h"
#include "CRegionManagerBody.h"
#include <iostream>

CGObjPC* CGObjManager::GetObjPCByCharName16(const char* CharName16)
{
	// 0x00400000 (gs base addr) + 0x85E20 (offset);
	return reinterpret_cast<CGObjPC * (__cdecl*)(const char*)>(0x00485E20)(CharName16);
}

CGObjMob* CGObjManager::CreateMob(uint32_t RefObjId, uint32_t GameWorldId, uint16_t RegionId, float PosX, float PosY, float PosZ, float GenerateRadius)
{
	// Create position
	NavInfo posNavInfo(PosInfo(RegionId, PosX, PosY, PosZ));

	// Check if can be solved
	if (!CRegionManagerBody::ResolveCellAndHeight(&posNavInfo))
	{
		printf(" * [CreateMob] Couldn't resolve cell & height. | [%d - %f, %f, %f]\n", RegionId, PosX, PosY, PosZ);
		return nullptr;
	}

	// Check if the data from object can be found
	const auto objBaseDataPtr = GetRefObjBaseData(RefObjId);
	if (!objBaseDataPtr)
	{
		printf(" * [CreateMob] Couldn't find the object data. | [%d]", RefObjId);
		return nullptr;
	}

	// Retrieve base data
	auto objCreepType = GetCreepType(objBaseDataPtr);
	auto objRefTacticPtr = GetRefTactic(objBaseDataPtr);
	auto objDir = rand() / 32767.f * 6.283185482025146f;

	CGObjMob* _eax;

	__asm
	{
		mov edx, 0x400000
		lea esi, [edx + 0x1F6EB0]	// base_addr + CreateObj(RVA)
		lea edi, [edx + 0x90B380]	// base_addr + CGameWorldMgr(RVA of Instance)
		lea eax, [posNavInfo]		// nav_info

		push 1										// unk
		push 0										// unk
		push 0										// unk
		push 0										// unk
		push objCreepType							// obj_creep_type
		push GenerateRadius							// generate_radius
		push objDir									// obj_direction
		push dword ptr ss : [objRefTacticPtr]		// obj_ref_tactic_ptr
		push 0										// obj_ref_nest_ptr
		push dword ptr ss : [objBaseDataPtr]		// obj_ref_base_data_ptr
		push 0										// unk
		push GameWorldId								// unk
		push edi									// CGameWorldMgr instance
		call esi
		mov _eax, eax
	}

	return _eax;
}

void* CGObjManager::GetRefObjBaseData(int RefObjId)
{
	void* _eax;

	__asm
	{
		mov edx, 0x400000
		lea edx, [edx + 0x260B0]

		mov edi, dword ptr ds : [0xD6AA14]
		push RefObjId
		call edx
		mov _eax, eax
	}
	return _eax;
}


int32_t CGObjManager::GetCreepType(void* RefObjBaseDataPtr)
{
	return *reinterpret_cast<uint8_t*>(reinterpret_cast<std::uintptr_t>(RefObjBaseDataPtr) + 0x89);
}

void* CGObjManager::GetRefTactic(void* RefObjBaseDataPtr)
{
	void* _eax;

	__asm
	{
		mov edx, 0x400000
		lea edx, [edx + 0x120D10]

		mov eax, dword ptr ss : [RefObjBaseDataPtr]
		call edx
		mov _eax, eax
	}
	return _eax;
}
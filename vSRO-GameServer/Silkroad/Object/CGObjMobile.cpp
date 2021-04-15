#include "CGObjMobile.h"

void CGObjMobile::AddBuff(uint32_t SkillId, uint32_t Seconds)
{
	// TO DO: More testing with all given params
	__asm
	{
		mov eax, 0x49A390
		mov esi, this
		push 0
		push 0
		push 0
		push 0
		push 0
		push 0
		push 0
		push 0
		push Seconds
		push SkillId
		push 0
		call eax
	}
}
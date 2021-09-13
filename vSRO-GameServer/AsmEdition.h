#include <Windows.h>
#pragma once

// Direct ASM injection
namespace AsmEdition
{
	// Jump back to the code flow from donating guild points
	static DWORD jmpAddr_DonateGP = 0x005C413A;
	// Hook on guild point increasing (0x005C4135)
	static _declspec(naked) void OnDonateGuildPoints()
	{
		// Rebuild asm
		__asm
		{
			mov ecx, dword ptr[eax + 0x3c] // rebuild
			add ecx, esi // rebuild
			cmp ecx, 0x7FFFFFFF // compare ecx with int.MaxValue
			jbe _continue // go to _continue if ecx <= int.MaxValue
			mov ecx, 0x7FFFFFFF // set ecx as int.MaxValue
			jmp _continue // go to _continue
		}
		// Contine code flow
		_continue:
			__asm jmp jmpAddr_DonateGP;
	}
}
#include <Windows.h>
#pragma once
// Direct ASM injection
namespace AsmEdition
{
	// Jump back to the code flow from donating guild points
	static DWORD jmpAddr_DonateGP_01 = 0x004364F3;
	static DWORD jmpAddr_DonateGP_02 = 0x004365D2;
	// Handler to catch guild point increasing hook and edit directly with asm
	static _declspec(naked) void OnDonateGuildPoints()
	{
		// Rebuild asm
		__asm
		{
			mov ecx, dword ptr[eax + 0x3c] // rebuild
			add ecx, edi // rebuild
			cmp ecx, 0x7FFFFFFF // compare ecx with int.MaxValue
			jbe _continue // go to _continue if ecx <= int.MaxValue
			mov ecx, 0x7FFFFFFF // set ecx as int.MaxValue
			mov ax, 0x1ABC // set ax with 0x1ABC as custom error code
			jmp _skip_code // go to _skip_code
		}
		// Contine code flow
	_continue:
		__asm jmp jmpAddr_DonateGP_01;
	_skip_code:
		__asm jmp jmpAddr_DonateGP_02;
	}
	// Jump back to the code flow from error code on guild points
	static DWORD jmpAddr_DonateErrCode_01 = 0x00438B99;
	static DWORD jmpAddr_DonateErrCode_02 = 0x00438BA1;
	static DWORD jmpAddr_DonateErrCode_03 = 0x00438B71;
	// Handler to catch donating guild point error code hook and edit directly with asm
	static _declspec(naked) void OnDonateGuildPointsErrorCode()
	{
		// Rebuild asm
		__asm
		{
			movzx eax, ax // rebuild
			cmp ax, 0x1 // rebuild
			je _first_case // rebuild
			cmp ax, 0x1ABC // compare ax with 0x1ABC as custom error code
			je _skip_message // go to _skip_message
			jmp _continue // go to _continue
		}
		// Contine code flow
	_first_case:
		__asm jmp jmpAddr_DonateErrCode_01;
	_skip_message:
		__asm jmp jmpAddr_DonateErrCode_02;
	_continue:
		__asm jmp jmpAddr_DonateErrCode_03;
	}
	// Jump back to the code flow from error msg on guild points
	static DWORD jmpAddr_DonateErrMsg_01 = 0x0043AA60;
	static DWORD jmpAddr_DonateErrMsg_02 = 0x0043AA03;
	static DWORD jmpAddr_DonateErrMsg_03 = 0x0043AA26;
	static DWORD callAddr_DonateErrMsg_01 = 0x00438010;
	// Handler to catch donating guild point error msg hook and edit directly with asm
	static _declspec(naked) void OnDonateGuildPointsErrorMsg()
	{
		// Rebuild asm
		__asm
		{
			movzx eax, ax // rebuild
			cmp ax, 0x1 // rebuild
			mov dword ptr ss : [esp + 0x1C] , eax // rebuild
			je _first_case // rebuild
			cmp ax, 0x1ABC // compare ax with 0x1ABC as custom error code
			jne _continue // go to _continue
			mov eax, dword ptr ss : [esp + 0x10]
			push eax
			call callAddr_DonateErrMsg_01
			add esp, 0x4
			jmp _skip_stuffs
		}
		// Contine code flow
	_first_case:
		__asm jmp jmpAddr_DonateErrMsg_01;
	_continue:
		__asm jmp jmpAddr_DonateErrMsg_02;
	_skip_stuffs:
		__asm jmp jmpAddr_DonateErrMsg_03;
	}
}
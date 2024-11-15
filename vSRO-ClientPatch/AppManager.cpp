#include "AppManager.h"
// Console stuffs
#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
// Utils
#include "Utils/Memory/Process.h"
#pragma warning(disable:4244) // Bitwise operations warnings

/// Static stuffs
bool AppManager::m_IsInitialized;
void AppManager::Initialize()
{
	if (!m_IsInitialized)
	{
		m_IsInitialized = true;
		InitPatchValues();
	}
}
void AppManager::InitPatchValues()
{
	std::cout << " * Initializing patches..." << std::endl;

	// SERVER_LEVEL_MAX
	uint8_t levelMax = 110;
	WriteMemoryValue<uint8_t>(0x008A99A2 + 2, levelMax); // Level up limit
	WriteMemoryValue<uint8_t>(0x0069C7C8 + 1, levelMax); // Mastery limit
	//WriteMemoryValue<uint8_t>(0x0073940E + 1, levelMax); // Party Match (Auto match)
	//WriteMemoryValue<uint8_t>(0x00739453 + 1, levelMax); // Party Match (Auto match)
	WriteMemoryValue<uint8_t>(0x0073AFAE + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x0073B013 + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x0073B030 + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x0073FA4C + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x0073FAAF + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x0073FACC + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x009448B1 + 6, levelMax); // Skill limit
	
	// SERVER_STALL_PRICE_LIMIT
	uint64_t priceLimit = 9999999999;
	WriteMemoryValue<uint8_t>(0x005DF9DE + 1, priceLimit >> 32);
	WriteMemoryValue<uint32_t>(0x005DF9E0 + 1, priceLimit);
	WriteMemoryValue<uint8_t>(0x005EA683 + 1, priceLimit >> 32);
	WriteMemoryValue<uint32_t>(0x005EA685 + 1, priceLimit);
	WriteMemoryValue<uint8_t>(0x006B2003 + 1, priceLimit >> 32);
	WriteMemoryValue<uint32_t>(0x006B2005 + 1, priceLimit);
	WriteMemoryValue<uint8_t>(0x006BA765 + 1, priceLimit >> 32);
	WriteMemoryValue<uint32_t>(0x006BA767 + 1, priceLimit);

	// SERVER_RESURRECT_SAME_POINT_LEVEL_MAX
	uint8_t level = 10;
	WriteMemoryValue<uint8_t>(0x00645688 + 1, level); // Show message
	WriteMemoryValue<uint8_t>(0x00797E21 + 1, level); // Unlock action

	// SERVER_BEGINNER_MARK_LEVEL_MAX
	bool displayAlways = false;
	if (displayAlways)
		for (int i = 0; i < 6; i++)
			WriteMemoryValue<uint8_t>(0x009DED3D + i, 0x90); // nop

	// RACE_CH_TOTAL_MASTERIES
	uint32_t masteries_CH = 330;
	WriteMemoryValue<uint32_t>(0x006A51BC + 1, masteries_CH);
	WriteMemoryValue<uint32_t>(0x006AA4C3 + 1, masteries_CH);
	
	// RACE_EU_TOTAL_MASTERIES
	uint32_t masteries_EU = 220;
	WriteMemoryValue<uint32_t>(0x006A5197 + 1, masteries_EU);
	WriteMemoryValue<uint32_t>(0x006A51A2 + 1, masteries_EU);
	WriteMemoryValue<uint32_t>(0x006AA498 + 1, masteries_EU);
	WriteMemoryValue<uint32_t>(0x006AA4A3 + 1, masteries_EU);
	
	// GUILD_MEMBERS_LIMIT
	WriteMemoryValue<uint32_t>(0x00D8C438, 15); // Lv.1
	WriteMemoryValue<uint32_t>(0x00D92388, 15);
	WriteMemoryValue<uint32_t>(0x00D8C43C, 20); // Lv.2
	WriteMemoryValue<uint32_t>(0x00D9238C, 20);
	WriteMemoryValue<uint32_t>(0x00D8C440, 25); // Lv.3
	WriteMemoryValue<uint32_t>(0x00D92390, 25);
	WriteMemoryValue<uint32_t>(0x00D8C444, 35); // Lv.4
	WriteMemoryValue<uint32_t>(0x00D92394, 35);
	WriteMemoryValue<uint32_t>(0x00D8C448, 50); // Lv.5
	WriteMemoryValue<uint32_t>(0x00D92398, 50);

	// GUILD_UNION_CHAT_PARTICIPANTS
	uint8_t unionChatParticipants = 12;
	WriteMemoryValue<uint8_t>(0x005AC538 + 1, unionChatParticipants);

	// ACCOUNT_CHARACTERS_MAX ("SRO_VT_SHARD.dbo._AddNewChar" MODIFICATION REQUIRED)
	uint8_t charactersPerAccount = 4;
	WriteMemoryValue<uint8_t>(0x0085DE67 + 6, charactersPerAccount);

	/// EXTRAS

	// Cooldown from Universal Pills (ms)
	WriteMemoryValue<uint32_t>(0x0087ABD3 + 1, 1000);
	// Cooldown from Purification Pills (ms)
	WriteMemoryValue<uint32_t>(0x0087AC02 + 1, 20000);
	
	// Disable zoom limit check
	WriteMemoryValue<uint8_t>(0x0077B4F3 + 2, 0);

	// Range to auto select monsters (range)
	WriteMemoryValue<float>(0x00D990A8, 750.0);

	// Background sight range
	WriteMemoryValue<float>(0x00DE4C5C, 1500.0f); // 0
	WriteMemoryValue<float>(0x00DE34C0, 2500.0f); // 1
	WriteMemoryValue<float>(0x00DE4C58, 3500.0f); // 2
	WriteMemoryValue<float>(0x00DE4C54, 4500.0f); // 3
	WriteMemoryValue<float>(0x00DE4C50, 5500.0f); // 4

	// Nude characters
	bool useNudity = false;
	if (useNudity)
		for (int i = 0; i < 2; i++)
			WriteMemoryValue<uint8_t>(0x00A5CE5B + i, 0x90); // nop

	// Loading screen image resolution
	WriteMemoryValue<uint32_t>(0x0086D405 + 4, 400); // 400
	WriteMemoryValue<uint32_t>(0x0086D40D + 4, 148); // 148

	// XTrap
	bool useXTrap = false;
	if(!useXTrap)
	{
		// Skip "Silkroad" mutex probably
		WriteMemoryValue<uint8_t>(0x00844AEF, 0xEB); // je? jne? -> jmp
		// Remove call to "Silkload.dat"
		for (int i = 0; i < 5; i++)
			WriteMemoryValue<uint8_t>(0x008328C3 + i, 0x90); // nop
		// Remove a lot stuffs I don't even know
		for (int i = 0x00B7AEF0; i <= 0x00B7C3EF; i++)
			WriteMemoryValue<uint8_t>(i, 0x90); // nop
	}

	// Multiclient
	bool useMulticlient = false;
	if(useMulticlient)
	{
		bool isLauncherRequired = true;
		if(!isLauncherRequired)
		{
			// Skip "Silkroad.exe" running requirement
			WriteMemoryValue<uint8_t>(0x008329EB, 0xEB); // jne -> jmp
			// Skip mutexes required from launcher; "Silkroad Online Launcher" and "Ready"
			WriteMemoryValue<uint8_t>(0x00830C67, 0xEB); // je -> jmp
		}
		// Skip mutex check for "Silkroad Client" already executed
		WriteMemoryValue<uint8_t>(0x0083297F, 0xEB); // jne -> jmp
	}
	
	// Berserker Title color
	WriteMemoryValue<uint32_t>(0x009C28ED + 1, 0xFFFFE65B);
	// [GM] Text color
	WriteMemoryValue<uint32_t>(0x009CFDD2 + 1, 0xFFFFD87A);
	
	// Item SEAL color
	WriteMemoryValue<uint32_t>(0x0066FC74 + 1, 0xFFFFD953);
	// Item SET color
	WriteMemoryValue<uint32_t>(0x0066FCC8 + 1, 0xFF6CE675); // Color below Seal of...
	WriteMemoryValue<uint32_t>(0x0067A097 + 1, 0xFF6CE675); // Color above set options
	WriteMemoryValue<uint32_t>(0x0067A2F2 + 1, 0xFF6CE675); // Color below set options
	WriteMemoryValue<uint32_t>(0x00675C4F + 3 + 4, 0xFF6CE675); // Color below set options

}
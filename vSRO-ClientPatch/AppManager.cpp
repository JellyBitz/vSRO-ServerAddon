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
	WriteMemoryValue<uint8_t>(0x0073940E + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x00739453 + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x0073AFAE + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x0073B013 + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x0073B030 + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x0073FA4C + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x0073FAAF + 1, levelMax); // Party Match
	WriteMemoryValue<uint8_t>(0x0073FACC + 1, levelMax); // Party Match

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
	if(displayAlways)
	{
		for(int i = 0; i < 6; i++)
		{
			WriteMemoryValue<uint8_t>(0x009DED3D, 0x90); // nop
		}
	}

	// RACE_CH_TOTAL_MASTERIES
	uint8_t masteries_CH = 330;
	WriteMemoryValue<uint8_t>(0x006A51BC + 1, masteries_CH);
	WriteMemoryValue<uint8_t>(0x006AA4C3 + 1, masteries_CH);
	
	// RACE_EU_TOTAL_MASTERIES
	uint8_t masteries_EU = 330;
	WriteMemoryValue<uint8_t>(0x006A5197 + 1, masteries_EU);
	WriteMemoryValue<uint8_t>(0x006A51A2 + 1, masteries_EU);
	WriteMemoryValue<uint8_t>(0x006AA498 + 1, masteries_EU);
	WriteMemoryValue<uint8_t>(0x006AA4A3 + 1, masteries_EU);

	// GUILD_MEMBERS_LIMIT
	WriteMemoryValue<uint32_t>(0x00D8C438, 15); // level 1
	WriteMemoryValue<uint32_t>(0x00D8C43C, 20); // level 2
	WriteMemoryValue<uint32_t>(0x00D8C440, 25); // level 3
	WriteMemoryValue<uint32_t>(0x00D8C444, 35); // level 4
	WriteMemoryValue<uint32_t>(0x00D8C448, 50); // level 5
	WriteMemoryValue<uint32_t>(0x00D92388, 15); // level 1
	WriteMemoryValue<uint32_t>(0x00D9238C, 20); // level 2
	WriteMemoryValue<uint32_t>(0x00D92390, 25); // level 3
	WriteMemoryValue<uint32_t>(0x00D92394, 35); // level 4
	WriteMemoryValue<uint32_t>(0x00D92398, 50); // level 5

	// GUILD_UNION_CHAT_PARTICIPANTS
	uint8_t unionChatParticipants = 12;
	WriteMemoryValue<uint8_t>(0x005AC538 + 1, unionChatParticipants);

	// ACCOUNT_CHARACTERS_MAX ("SRO_VT_SHARD.dbo._AddNewChar" MODIFICATION REQUIRED)
	uint8_t charactersPerAccount = 4;
	WriteMemoryValue<uint8_t>(0x0085DE67 + 6, charactersPerAccount);

	/// EXTRAS
	// Cooldown for Universal Pills (ms)
	WriteMemoryValue<uint32_t>(0x0087ABD3 + 1, 1000);
	// Cooldown for Purification Pills (ms)
	WriteMemoryValue<uint32_t>(0x0087AC02 + 1, 20000);
	// Disable zoom limit check
	WriteMemoryValue<uint8_t>(0x0077B4F3 + 2, 0);
	// Range to auto select monsters (range)
	WriteMemoryValue<float>(0x00D990A8, 750.0);
}
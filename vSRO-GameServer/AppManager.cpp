#include "AppManager.h"
// Console stuffs
#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
// Thread signal
#include <csignal>
#include <sstream>
#include <exception>
// Gameserver stuffs
#include "Silkroad/CGObjManager.h"
// Utils
#include "Utils/IO/SimpleIni.h"
#include "Utils/Memory/Process.h"
#include "Utils/Memory/hook.h"
#pragma warning(disable:4244) // Bitwise operations warnings

/// Static stuffs
bool AppManager::m_IsInitialized;
DatabaseLink AppManager::m_dbLink, AppManager::m_dbLinkHelper, AppManager::m_dbUniqueLog;
bool AppManager::m_IsRunningDatabaseFetch;

void AppManager::Initialize()
{
	if (!m_IsInitialized)
	{
		m_IsInitialized = true;
		InitConfigFile();
		InitDebugConsole();
		InitPatchValues();
		InitHooks();
		InitDatabaseFetch();
	}
}
void AppManager::InitConfigFile()
{
	CSimpleIniA ini;
	// Try to load it or create a new one
	if (ini.LoadFile("vSRO-GameServer.ini") != SI_Error::SI_OK)
	{
		ini.SetSpaces(false);
		// Sql
		ini.SetValue("Sql", "HOST", "localhost", "; SQL Server address or name");
		ini.SetValue("Sql", "PORT", "1433", "; SQL Server port");
		ini.SetValue("Sql", "USER", "sa", "; Username credential");
		ini.SetValue("Sql", "PASS", "1234", "; Password credential");
		ini.SetValue("Sql", "DB_SHARD", "SRO_VT_SHARD", "; Name used for the specified silkroad database");
		ini.SetValue("Sql", "DB_LOG", "SRO_VT_SHARDLOG", "; Name used for the specified silkroad database");
		// Memory
		ini.SetLongValue("Server", "LEVEL_MAX", 110, "; Maximum level that can be reached on server");
		ini.SetLongValue("Server", "STALL_PRICE_LIMIT", 9999999999, "; Maximum price that can be stalled");
		ini.SetLongValue("Server", "PARTY_MOB_MEMBERS_REQUIRED", 2, "; Party members required to find monsters party type");
		ini.SetLongValue("Server", "PARTY_MOB_SPAWN_PROBABILITY", 50, "; % Probability for party mob spawns");
		ini.SetLongValue("Server", "PENALTY_DROP_PROBABILITY", 5, "; % Probability to drop an item when a player dies");
		ini.SetLongValue("Server", "PK_LEVEL_REQUIRED", 20, "; Level required to kill other player");
		ini.SetLongValue("Job", "LEVEL_MAX", 7, "; Maximum level that can be reached on job suit");
		ini.SetLongValue("Race", "CH_TOTAL_MASTERIES", 330, "; Masteries amount Chinese will obtain");
		ini.SetLongValue("Guild", "MEMBERS_LIMIT_LEVEL1", 15, "; Guild members capacity at level 1");
		ini.SetLongValue("Guild", "MEMBERS_LIMIT_LEVEL2", 20, "; Guild members capacity at level 2");
		ini.SetLongValue("Guild", "MEMBERS_LIMIT_LEVEL3", 25, "; Guild members capacity at level 3");
		ini.SetLongValue("Guild", "MEMBERS_LIMIT_LEVEL4", 35, "; Guild members capacity at level 4");
		ini.SetLongValue("Guild", "MEMBERS_LIMIT_LEVEL5", 50, "; Guild members capacity at level 5");
		ini.SetLongValue("Guild", "STORAGE_SLOTS_MIN", 30, "; Storage slots at first level");
		ini.SetLongValue("Guild", "STORAGE_SLOTS_INCREASE", 30, "; Storage slots increased per level");
		ini.SetLongValue("Guild", "UNION_LIMIT", 8, "; Union participants limit");
		ini.SetLongValue("Guild", "UNION_CHAT_PARTICIPANTS", 12, "; Union chat participants allowed by guild");
		ini.SetLongValue("Academy", "GRADUATE_BEGINNER_LEVEL", 40, "; Graduation level for the beginner members");
		ini.SetLongValue("Academy", "DISBAND_PENALTY_TIME", 604800, "; Penalty time (seconds) to create again the academy group");
		ini.SetValue("Event","CTF_ITEM_REWARD","ITEM_ETC_E070919_TROPHY","; Item reward from Capture The Flag");
		ini.SetLongValue("Event", "CTF_ITEM_REWARD_AMOUNT", 1, "; Amount to obtain per every kill");
		ini.SetValue("Event","BA_ITEM_REWARD","ITEM_ETC_ARENA_COIN","; Item reward from Battle Arena");
		ini.SetLongValue("Event", "BA_ITEM_REWARD_GJ_W_AMOUNT", 7, "; Amount to obtain winning on Guild/Job mode");
		ini.SetLongValue("Event", "BA_ITEM_REWARD_GJ_L_AMOUNT", 2, "; Amount to obtain loosing");
		ini.SetLongValue("Event", "BA_ITEM_REWARD_PR_W_AMOUNT", 7, "; Amount to obtain winning on Party/Random mode");
		ini.SetLongValue("Event", "BA_ITEM_REWARD_PR_L_AMOUNT", 2, "; Amount to obtain loosing");
		ini.SetLongValue("Fix", "AGENT_SERVER_CAPACITY", 1000, "; Set capacity supported by the connected agent server");
		ini.SetBoolValue("Fix", "HIGH_RATES_CONFIG", true, "; Fix rates (ExpRatio/1000) to use higher values than 2500");
		ini.SetBoolValue("Fix", "UNIQUE_LOGS", true, "; Log unique spawn/killed into _AddLogChar as EventID = 32/33");
		ini.SetBoolValue("Fix", "DISABLE_GREEN_BOOK", true, "; Disable buff with the green book");
		ini.SetBoolValue("Fix", "DISABLE_MSGBOX_SILK_GOLD_PRICE", true, "; Disable messages about \"register silk/gold price.\"");
		// App
		ini.SetBoolValue("App", "DEBUG_CONSOLE", true, "; Attach debug console");
		// Save it
		ini.SaveFile("vSRO-GameServer.ini");
	}
}
void AppManager::InitDebugConsole()
{
	// Load file
	CSimpleIniA ini;
	ini.LoadFile("vSRO-GameServer.ini");

	// Check if console has been deactivated
	if (ini.GetBoolValue("App", "DEBUG_CONSOLE", true))
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		freopen("CONIN$", "r", stdin);
	}
}
void AppManager::InitHooks()
{
	std::cout << " * Initializing hooks..." << std::endl;

	// Load file
	CSimpleIniA ini;
	ini.LoadFile("vSRO-GameServer.ini");

	// Uniques
	if (ini.GetBoolValue("Fix","UNIQUE_LOGS",true))
	{
		// Create connection string
		std::wstringstream connString;
		connString << "DRIVER={SQL Server};";
		connString << "SERVER=" << ini.GetValue("Sql", "HOST", "localhost") << ", " << ini.GetValue("Sql", "PORT", "1433") << ";";
		connString << "DATABASE=" << ini.GetValue("Sql", "DB_LOG", "SRO_VT_SHARDLOG") << ";";
		connString << "UID=" << ini.GetValue("Sql", "USER", "sa") << ";";
		connString << "PWD=" << ini.GetValue("Sql", "PASS", "1234") << ";";

		if (m_dbUniqueLog.sqlConn.Open((SQLWCHAR*)connString.str().c_str()) && m_dbUniqueLog.sqlCmd.Open(m_dbUniqueLog.sqlConn))
		{
			if (replaceOffset(0x00414DB0, addr_from_this(&AppManager::OnUniqueSpawnMsg)))
			{
				std::cout << " - OnUniqueSpawnMsg" << std::endl;
			}
			if (replaceOffset(0x00414BA9, addr_from_this(&AppManager::OnUniqueKilledMsg)))
			{
				std::cout << " - OnUniqueKilledMsg" << std::endl;
			}
		}
	}
}
void AppManager::OnUniqueSpawnMsg(uint32_t LogType, const char* Message, const char* UniqueCodeName, uint16_t RegionId, uint32_t unk01, uint32_t unk02)
{
	// Build query
	std::wstringstream qExecLog;
	qExecLog << "EXEC dbo._AddLogChar";
	qExecLog << " 0,32," << RegionId << ",0,'','" << UniqueCodeName << "'";

	// Try execute it
	m_dbUniqueLog.sqlCmd.ExecuteQuery((SQLWCHAR*)qExecLog.str().c_str());
	m_dbUniqueLog.sqlCmd.Clear();

	// Avoid call original function since it's about printing into gameserver logs
}
void AppManager::OnUniqueKilledMsg(uint32_t LogType, const char* Message, const char* UniqueCodeName, const char* CharName)
{
	auto player = CGObjManager::GetObjPCByCharName16(CharName);
	auto pos = player->GetPosition();
	// Build query
	std::wstringstream qExecLog;
	qExecLog << "EXEC dbo._AddLogChar";
	qExecLog << " " << player->GetCharID() << ",33," << pos.RegionId << "," << pos.UnkUShort01 << ",'" << (int)pos.PosX << "," << (int)pos.PosY << "," << (int)pos.PosZ << "','" << UniqueCodeName << "'";

	// Try execute it
	m_dbUniqueLog.sqlCmd.ExecuteQuery((SQLWCHAR*)qExecLog.str().c_str());
	m_dbUniqueLog.sqlCmd.Clear();

	// Avoid call original function since it's about printing into gameserver logs
}
void AppManager::InitPatchValues()
{
	std::cout << " * Initializing patches..." << std::endl;

	// Load file
	CSimpleIniA ini;
	ini.LoadFile("vSRO-GameServer.ini");

	// buffers
	uint8_t byteValue;
	uint32_t uintValue;

	// Server
	if (ReadMemoryValue<uint8_t>(0x004E52C7 + 2, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Server", "LEVEL_MAX", 110);
		printf(" - SERVER_LEVEL_MAX (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x004E52C7 + 2, newValue); // Character
		WriteMemoryValue<uint8_t>(0x004D641B + 3, newValue); // Pet
		WriteMemoryValue<uint16_t>(0x004E5471 + 4, (newValue - 1) * 4); // Exp bug fix
	}
	if (ReadMemoryValue<uint8_t>(0x00471B00 + 2, byteValue) && ReadMemoryValue<uint32_t>(0x00471B07 + 1, uintValue))
	{
		uint64_t newValue = ini.GetLongValue("Server", "STALL_PRICE_LIMIT", 9999999999);
		newValue = newValue & 0xFFFFFFFFFF; // Limit value to 5 bytes
		printf(" - SERVER_STALL_PRICE_LIMIT (%llu) -> (%llu)\r\n", ((uint64_t)byteValue << 32) | uintValue, newValue);
		// Stall
		WriteMemoryValue<uint8_t>(0x00471B00 + 2, newValue >> 32);
		WriteMemoryValue<uint32_t>(0x00471B07 + 1, newValue);
		WriteMemoryValue<uint8_t>(0x00471B00 + 2, newValue >> 32);
		WriteMemoryValue<uint32_t>(0x00471B07 + 1, newValue);
		WriteMemoryValue<uint8_t>(0x00472FF5 + 2, newValue >> 32);
		WriteMemoryValue<uint32_t>(0x00473008 + 1, newValue);
		WriteMemoryValue<uint8_t>(0x0047ABD8 + 2, newValue >> 32);
		WriteMemoryValue<uint32_t>(0x0047ABE3 + 1, newValue);
		// Exchange will take the highest UX value
		if (ReadMemoryValue<uint32_t>(0x00480F5E + 4, uintValue))
		{
			if (newValue > 4000000000u)
				newValue = 4000000000u;
			printf(" - SERVER_EXCHANGE_GOLD_LIMIT (%u) -> (%u)\r\n", uintValue, (uint32_t)newValue);
			WriteMemoryValue<uint32_t>(0x00480F5E + 4, newValue);
			WriteMemoryValue<uint32_t>(0x004D8F1A + 2, newValue);
			WriteMemoryValue<uint32_t>(0x004D8F22 + 2, newValue);
			WriteMemoryValue<uint32_t>(0x004F7734 + 2, newValue);
			WriteMemoryValue<uint32_t>(0x004F7746 + 4, newValue);
		}
	}
	if(ReadMemoryValue<uint8_t>(0x00558F20 + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Server", "PARTY_MOB_MEMBERS_REQUIRED", 2);
		printf(" - SERVER_PARTY_MOB_MEMBERS_REQUIRED (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x00558F20 + 4, newValue);
	}
	if (ReadMemoryValue<uint8_t>(0x005608E2 + 2, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Server", "PARTY_MOB_SPAWN_PROBABILITY", 50);
		printf(" - SERVER_PARTY_MOB_SPAWN_PROBABILITY (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x005608E2 + 2, newValue);
	}
	if(ReadMemoryValue<uint8_t>(0x005295DA + 1, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Server", "PK_LEVEL_REQUIRED", 20);
		printf(" - SERVER_PK_LEVEL_REQUIRED (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x005295DA + 1, newValue);
	}
	if (ReadMemoryValue<uint8_t>(0x004E696D + 1, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Server", "PENALTY_DROP_PROBABILITY", 5);
		printf(" - SERVER_PENALTY_DROP_PROBABILITY (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x004E696D + 1, newValue);
	}

	// Job
	if (ReadMemoryValue<uint8_t>(0x0060DE69 + 3, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Job", "LEVEL_MAX", 7);
		printf(" - JOB_LEVEL_MAX (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x0060DE69 + 3, newValue);
	}
	
	// Race
	if (ReadMemoryValue<uint32_t>(0x0059C5E6 + 1, uintValue))
	{
		uint32_t newValue = ini.GetLongValue("Race", "CH_TOTAL_MASTERIES", 330);
		printf(" - RACE_CH_TOTAL_MASTERIES (%u) -> (%u)\r\n", uintValue, newValue);
		WriteMemoryValue<uint32_t>(0x0059C5E6 + 1, newValue);
	}

	// Guild
	{
		uint32_t addr = 0x00ADE8DC;
		if (ReadMemoryValue<uint32_t>(addr, uintValue))
		{
			uint32_t newValue = ini.GetLongValue("Guild", "MEMBERS_LIMIT_LEVEL1", 15);
			printf(" - GUILD_MEMBERS_LIMIT_LEVEL1 (%u) -> (%u)\r\n", uintValue, newValue);
			WriteMemoryValue<uint32_t>(addr, newValue);
		}
		if (ReadMemoryValue<uint32_t>(addr + 4, uintValue))
		{
			uint32_t newValue = ini.GetLongValue("Guild", "MEMBERS_LIMIT_LEVEL2", 20);
			printf(" - GUILD_MEMBERS_LIMIT_LEVEL2 (%u) -> (%u)\r\n", uintValue, newValue);
			WriteMemoryValue<uint32_t>(addr + 4, newValue);
		}
		if (ReadMemoryValue<uint32_t>(addr + 8, uintValue))
		{
			uint32_t newValue = ini.GetLongValue("Guild", "MEMBERS_LIMIT_LEVEL3", 25);
			printf(" - GUILD_MEMBERS_LIMIT_LEVEL3 (%u) -> (%u)\r\n", uintValue, newValue);
			WriteMemoryValue<uint32_t>(addr + 8, newValue);
		}
		if (ReadMemoryValue<uint32_t>(addr + 12, uintValue))
		{
			uint32_t newValue = ini.GetLongValue("Guild", "MEMBERS_LIMIT_LEVEL4", 35);
			printf(" - GUILD_MEMBERS_LIMIT_LEVEL4 (%u) -> (%u)\r\n", uintValue, newValue);
			WriteMemoryValue<uint32_t>(addr + 12, newValue);
		}
		if (ReadMemoryValue<uint32_t>(addr + 16, uintValue))
		{
			uint32_t newValue = ini.GetLongValue("Guild", "MEMBERS_LIMIT_LEVEL5", 50);
			printf(" - GUILD_MEMBERS_LIMIT_LEVEL5 (%u) -> (%u)\r\n", uintValue, newValue);
			WriteMemoryValue<uint32_t>(addr + 16, newValue);
		}
	}
	if (ReadMemoryValue<uint32_t>(0x00C6B5F8, uintValue))
	{
		uint32_t newValue = ini.GetLongValue("Guild", "STORAGE_SLOTS_MIN", 30);
		printf(" - GUILD_STORAGE_SLOTS_MIN (%d) -> (%d)\r\n", uintValue, newValue);
		WriteMemoryValue<uint32_t>(0x00C6B5F8, newValue);
		// Get value increased on second level
		uint32_t increaseValue;
		if (ReadMemoryValue<uint32_t>(0x00C6B5F8 + 4, increaseValue))
		{
			uint32_t increaseNewValue = ini.GetLongValue("Guild", "STORAGE_SLOTS_INCREASE", 30);
			printf(" - GUILD_STORAGE_SLOTS_INCREASE (%d) -> (%d)\r\n", increaseValue-uintValue, increaseNewValue);
			for(int i = 0; i < 3; i++)
				WriteMemoryValue<uint32_t>(0x00C6B5F8 + 4 + (i*4), newValue + (i+1)*increaseNewValue);
		}
	}
	if (ReadMemoryValue<uint8_t>(0x005B8EA1 + 1, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Guild", "UNION_LIMIT", 8);
		printf(" - GUILD_UNION_LIMIT (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x005B8EA1 + 1, newValue);
	}
	if (ReadMemoryValue<uint8_t>(0x005C4B42 + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Guild", "UNION_CHAT_PARTICIPANTS", 12);
		printf(" - GUILD_UNION_CHAT_PARTICIPANTS (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x005C4B42 + 4, newValue);
	}

	// Academy
	if (ReadMemoryValue<uint8_t>(0x00519883 + 2, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Academy", "GRADUATE_BEGINNER_LEVEL", 40);
		printf(" - ACADEMY_GRADUATE_BEGINNER_LEVEL (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x00519883 + 2, newValue);
		WriteMemoryValue<uint8_t>(0x005196CD + 1, newValue);
	}
	if (ReadMemoryValue<uint32_t>(0x005DD36A + 1, uintValue))
	{
		uint32_t newValue = ini.GetLongValue("Academy", "DISBAND_PENALTY_TIME", 604800);
		printf(" - ACADEMY_DISBAND_PENALTY_TIME (%u) -> (%u)\r\n", uintValue, newValue);
		WriteMemoryValue<uint32_t>(0x005DD36A + 1, newValue);
		WriteMemoryValue<uint32_t>(0x00651C7A + 2, newValue);
	}

	// Event
	{
		const char* newValue = ini.GetValue("Event", "CTF_ITEM_REWARD", "");
		auto newValueLen = strlen(newValue);
		// Change value if it's not empty and the CodeName is shorter than 128 bytes
		if (newValueLen != 0 && newValueLen <= 128)
		{
			auto currentValue = ReadMemoryString(0x00646D42 + 1);
			printf(" - EVENT_CTF_ITEM_REWARD (%s) -> (%s)\r\n", currentValue.c_str(), newValue);
			// Overwrites new char* into empty memory
			WriteMemoryString(0x00AD8FFD - 130, newValue);
			// Set char* pointer to the new value
			WriteMemoryValue<uint32_t>(0x00646D42 + 1, 0x00AD8FFD - 130);
			WriteMemoryValue<uint32_t>(0x005F19A9 + 1, 0x00AD8FFD - 130);
			WriteMemoryValue<uint32_t>(0x00876935 + 6, 0x00AD8FFD - 130);
		}
	}
	if (ReadMemoryValue<uint8_t>(0x00646C93 + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Event", "CTF_ITEM_REWARD_AMOUNT", 1);
		printf(" - EVENT_CTF_ITEM_REWARD_AMOUNT (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x00646C93 + 4, newValue);
	}
	{
		const char* newValue = ini.GetValue("Event", "BA_ITEM_REWARD", "");
		auto newValueLen = strlen(newValue);
		// Change value if it's not empty and the CodeName is shorter than 128 bytes
		if (newValueLen != 0 && newValueLen <= 128)
		{
			auto currentValue = ReadMemoryString(0x006691C6 + 1);
			printf(" - EVENT_BA_ITEM_REWARD (%s) -> (%s)\r\n", currentValue.c_str(), newValue);
			// Overwrites new char* into empty memory
			WriteMemoryString(0x00AD8FFD - 130 - 130, newValue);
			// Set char* pointer to the new value
			WriteMemoryValue<uint32_t>(0x006691C6 + 1, 0x00AD8FFD - 130 - 130);
		}
	}
	if (ReadMemoryValue<uint8_t>(0x00669158 + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Event", "BA_ITEM_REWARD_GJ_W_AMOUNT", 7);
		printf(" - EVENT_BA_ITEM_REWARD_GJ_W_AMOUNT (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x00669158 + 4, newValue);
	}
	if (ReadMemoryValue<uint8_t>(0x00669173 + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Event", "BA_ITEM_REWARD_GJ_L_AMOUNT", 2);
		printf(" - EVENT_BA_ITEM_REWARD_GJ_L_AMOUNT (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x00669173 + 4, newValue);
	}
	if (ReadMemoryValue<uint8_t>(0x0066915F + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Event", "BA_ITEM_REWARD_PR_W_AMOUNT", 7);
		printf(" - EVENT_BA_ITEM_REWARD_PR_W_AMOUNT (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x0066915F + 4, newValue);
	}
	if (ReadMemoryValue<uint8_t>(0x0066917A + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Event", "BA_ITEM_REWARD_PR_L_AMOUNT", 2);
		printf(" - EVENT_BA_ITEM_REWARD_PR_L_AMOUNT (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x0066917A + 4, newValue);
	}

	// Fixes
	if (ReadMemoryValue<uint32_t>(0x004744BC + 1, uintValue))
	{
		uint32_t newValue = ini.GetLongValue("Fix", "AGENT_SERVER_CAPACITY", 1000);
		printf(" - FIX_AGENT_SERVER_CAPACITY (%u) -> (%u)\r\n", uintValue, newValue);
		WriteMemoryValue<uint32_t>(0x004744BC + 1, newValue);
		WriteMemoryValue<uint32_t>(0x004744C7 + 1, newValue);
	}
	if (ini.GetBoolValue("Fix", "HIGH_RATES_CONFIG", true))
	{
		printf(" - FIX_HIGH_RATES_CONFIG\r\n");
		WriteMemoryValue<uint8_t>(0x0042714C + 2, 0x42); // ExpRatio
		WriteMemoryValue<uint8_t>(0x004271F5 + 2, 0x42); // ExpRatioParty
		WriteMemoryValue<uint8_t>(0x004272A0 + 2, 0x42); // DropItemRatio
		WriteMemoryValue<uint8_t>(0x00427349 + 2, 0x42); // DropGoldAmountCoef
	}
	if (ini.GetBoolValue("Fix", "DISABLE_GREEN_BOOK", true))
	{
		printf(" - FIX_DISABLE_GREEN_BOOK\r\n");
		for(int i = 0; i < 8; i++)
			WriteMemoryValue<uint8_t>(0x004142E2 + i, 0x90); // NOP
		for(int i = 0; i < 5; i++)
			WriteMemoryValue<uint8_t>(0x0041474D + i, 0x90); // NOP
	}
	if (ini.GetBoolValue("Fix", "DISABLE_MSGBOX_SILK_GOLD_PRICE", true))
	{
		printf(" - FIX_DISABLE_MSGBOX_SILK_GOLD_PRICE\r\n");
		WriteMemoryValue<uint8_t>(0x006A989E, 0xEB); // jne to jmp
		WriteMemoryValue<uint8_t>(0x006A98CB, 0xEB); // jne to jmp
	}
}
void AppManager::InitDatabaseFetch()
{
	// Check if this DLL is running into another gameserver process
	CreateMutexA(0, FALSE, "JellyBitz/vSRO-GameServer");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// Avoid create fetch connection
		return;
	}

	std::cout << " * Initializing database fetch to execute actions..." << std::endl;

	// Load file
	CSimpleIniA ini;
	ini.LoadFile("vSRO-GameServer.ini");

	// Create connection string
	std::wstringstream connString;
	connString << "DRIVER={SQL Server};";
	connString << "SERVER=" << ini.GetValue("Sql", "HOST", "localhost") << ", " << ini.GetValue("Sql", "PORT", "1433") << ";";
	connString << "DATABASE=" << ini.GetValue("Sql", "DB_SHARD", "SRO_VT_SHARD") << ";";
	connString << "UID=" << ini.GetValue("Sql", "USER", "sa") << ";";
	connString << "PWD=" << ini.GetValue("Sql", "PASS", "12341") << ";";

	if (m_dbLink.sqlConn.Open((SQLWCHAR*)connString.str().c_str()) && m_dbLink.sqlCmd.Open(m_dbLink.sqlConn)
		&& m_dbLinkHelper.sqlConn.Open((SQLWCHAR*)connString.str().c_str()) && m_dbLinkHelper.sqlCmd.Open(m_dbLinkHelper.sqlConn))
	{
		auto hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AppManager::DatabaseFetchThread, 0, 0, 0);
		//// Set thread as background process (below normal)
		//SetThreadPriority(hThread, -1);
	}
}
DWORD WINAPI AppManager::DatabaseFetchThread()
{
	std::cout << " - Waiting 1min before start fetching..." << std::endl;
	Sleep(60000);
	std::cout << " - Fetching started!" << std::endl;
	m_IsRunningDatabaseFetch = true;

	// Try to create table used to fetch
	std::wstringstream qCreateTable;
	qCreateTable << "IF OBJECT_ID(N'dbo._ExeGameServer', N'U') IS NULL";
	qCreateTable << " BEGIN";
	qCreateTable << " CREATE TABLE dbo._ExeGameServer";
	qCreateTable << " (ID INT IDENTITY(1,1) PRIMARY KEY,";
	qCreateTable << " Action_ID INT NOT NULL,";
	qCreateTable << " Action_Result SMALLINT NOT NULL DEFAULT 0,";
	qCreateTable << " CharName16 VARCHAR(64) NOT NULL,";
	qCreateTable << " Param01 VARCHAR(129),";
	qCreateTable << " Param02 BIGINT,";
	qCreateTable << " Param03 BIGINT,";
	qCreateTable << " Param04 BIGINT,";
	qCreateTable << " Param05 BIGINT,";
	qCreateTable << " Param06 BIGINT,";
	qCreateTable << " Param07 BIGINT,";
	qCreateTable << " Param08 BIGINT)";
	qCreateTable << " END";

	// Try execute query
	if (!m_dbLink.sqlCmd.ExecuteQuery((SQLWCHAR*)qCreateTable.str().c_str())) {
		m_IsRunningDatabaseFetch = false;
		return 0;
	}
	m_dbLink.sqlCmd.Clear();

	// Stops this thread loop on interruption/exit
	signal(SIGINT, [](int) {
		m_IsRunningDatabaseFetch = false;
	});

	// Start fetching actions without result
	std::wstringstream qSelectActions;
	qSelectActions << "SELECT ID, Action_ID, CharName16, Param01, Param02, Param03, Param04, Param05, Param06, Param07, Param08";
	qSelectActions << " FROM dbo._ExeGameServer";
	qSelectActions << " WHERE Action_Result = " << FETCH_ACTION_STATE::UNKNOWN;
	while (m_IsRunningDatabaseFetch)
	{
		// Try to execute query
		if (!m_dbLink.sqlCmd.ExecuteQuery((SQLWCHAR*)qSelectActions.str().c_str()))
			break;

		// Fetch one by one
		while (m_dbLink.sqlCmd.FetchData())
		{
			// Set default state
			FETCH_ACTION_STATE actionResult = FETCH_ACTION_STATE::SUCCESS;
			
			// Read required params
			SQLINTEGER cID, cActionID;
			char cCharName[64];
			m_dbLink.sqlCmd.GetData(1, SQL_C_ULONG, &cID, 0, NULL);
			m_dbLink.sqlCmd.GetData(2, SQL_C_ULONG, &cActionID, 0, NULL);
			m_dbLink.sqlCmd.GetData(3, SQL_C_CHAR, &cCharName, 64, 0);

			// Try to execute the action
			try {
				switch (cActionID)
				{
				case 1: // Add Item
				{
					// Read & check params
					char cParam01[128];
					SQLUINTEGER cParam02;
					SQLINTEGER cParam03;
					SQLUSMALLINT cParam04;
					if (m_dbLink.sqlCmd.GetData(4, SQL_C_CHAR, &cParam01, 128, 0)
						&& m_dbLink.sqlCmd.GetData(5, SQL_C_ULONG, &cParam02, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_ULONG, &cParam03, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(7, SQL_C_ULONG, &cParam04, 0, NULL))
					{
						// Check player existence
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
						{
							auto operationCode = player->AddItem(cParam01, cParam02, cParam03, cParam04);
							if(operationCode != 1)
							{
								std::cout << " > Unnexpected AddItem on [" << cCharName << "] Result [" << operationCode << "]" << std::endl;
								actionResult = FETCH_ACTION_STATE::FUNCTION_ERROR;
							}						
						}
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 2: // Update Gold
				{
					// Read params
					SQLBIGINT cParam02;
					if (m_dbLink.sqlCmd.GetData(5, SQL_C_LONG, &cParam02, 0, NULL))
					{
						// Check player existence
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
							player->UpdateGold(cParam02);
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 3: // Update Hwan level (Berserk title)
				{
					// Read params
					SQLUSMALLINT cParam02;
					if (m_dbLink.sqlCmd.GetData(5, SQL_C_USHORT, &cParam02, 0, NULL))
					{
						// Check player existence
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
							player->UpdateHwan(cParam02);
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 4: // Move Player
				{
					// Read params
					SQLUSMALLINT cParam02, cParam03, cParam04;
					SQLSMALLINT cParam05;
					if (m_dbLink.sqlCmd.GetData(5, SQL_C_USHORT, &cParam02, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_USHORT, &cParam03, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(7, SQL_C_SHORT, &cParam04, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(8, SQL_C_USHORT, &cParam05, 0, NULL))
					{
						// Check player existence
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
						{
							if(!player->MoveTo(cParam02, cParam03, cParam04, cParam05))
								actionResult = FETCH_ACTION_STATE::FUNCTION_ERROR;
						}
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 5: // Move Player through WorldId
				{
					// Read & check params
					SQLUSMALLINT cParam02, cParam03, cParam04, cParam06;
					SQLSMALLINT cParam05;
					if ( m_dbLink.sqlCmd.GetData(5, SQL_C_USHORT, &cParam02, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_USHORT, &cParam03, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(7, SQL_C_USHORT, &cParam04, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(8, SQL_C_SHORT, &cParam05, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(9, SQL_C_USHORT, &cParam06, 0, NULL))
					{
						// Check player existence
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
						{
							if(!player->MoveTo(cParam02 + 0x10000, cParam03, cParam04, cParam05, cParam06))
								actionResult = FETCH_ACTION_STATE::FUNCTION_ERROR;
						}
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;				
				} break;
				case 6: // Drop item near player
				{
					// Read & check params
					char cParam01[128];
					SQLUINTEGER cParam02;
					SQLUSMALLINT cParam03;
					if (m_dbLink.sqlCmd.GetData(4, SQL_C_CHAR, &cParam01, 128, 0)
						&& m_dbLink.sqlCmd.GetData(5, SQL_C_ULONG, &cParam02, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_USHORT, &cParam03, 0, NULL))
					{
						// Check player existence
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
							player->DropItem(cParam01, cParam02, cParam03);
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 7: // Transform item from inventory slot
				{ 
					// Read & check params
					char cParam01[128];
					SQLUSMALLINT cParam02;
					if (m_dbLink.sqlCmd.GetData(4, SQL_C_CHAR, &cParam01, 128, 0)
						&& m_dbLink.sqlCmd.GetData(5, SQL_C_USHORT, &cParam02, 0, NULL))
					{
						// Check player existence
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
						{
							if (!player->MutateItemAt(cParam02, cParam01))
								actionResult = FETCH_ACTION_STATE::FUNCTION_ERROR;
						}
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 8: // Force reloading player
				{ 
					// Check player existence
					CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
					if (player)
					{
						if(!player->Reload())
							actionResult = FETCH_ACTION_STATE::FUNCTION_ERROR;
					}
					else
						actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
				} break;
				case 9: // Add buff to player
				{
					// Read & check params
					SQLUINTEGER cParam02, cParam03;
					if (m_dbLink.sqlCmd.GetData(5, SQL_C_ULONG, &cParam02, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_ULONG, &cParam03, 0, NULL))
					{
						// Check player existence
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
							player->AddBuff(cParam02, cParam03);
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 10: // Create mob in map location
				{
					// Read & check params
					SQLUINTEGER cParam02;
					SQLUSMALLINT cParam03, cParam04, cParam06;
					SQLSMALLINT cParam05;
					if (m_dbLink.sqlCmd.GetData(5, SQL_C_ULONG, &cParam02, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_USHORT, &cParam03, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(7, SQL_C_USHORT, &cParam04, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(8, SQL_C_SHORT, &cParam05, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(9, SQL_C_USHORT, &cParam06, 0, NULL))
					{
						// Try to create mob
						if (!CGObjManager::CreateMob(cParam02, 0x10001, cParam03, cParam04, cParam05, cParam06, 10))
							actionResult = FETCH_ACTION_STATE::FUNCTION_ERROR;
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 11: // Create mob in map location through world id
				{
					// Read & check params
					SQLUINTEGER cParam02;
					SQLUSMALLINT cParam03, cParam04, cParam05, cParam07;
					SQLSMALLINT cParam06;
					if (m_dbLink.sqlCmd.GetData(5, SQL_C_ULONG, &cParam02, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_USHORT, &cParam03, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(7, SQL_C_USHORT, &cParam04, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(8, SQL_C_USHORT, &cParam05, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(9, SQL_C_SHORT, &cParam06, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(10, SQL_C_USHORT, &cParam07, 0, NULL))
					{
						// Try to create mob
						if (!CGObjManager::CreateMob(cParam02, cParam03 + 0x10000, cParam04, cParam05, cParam06, cParam07, 10))
							actionResult = FETCH_ACTION_STATE::FUNCTION_ERROR;
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 12: // Set body mode from player
				{
					SQLUSMALLINT cParam02;
					if (m_dbLink.sqlCmd.GetData(5, SQL_C_USHORT, &cParam02, 0, NULL))
					{
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
							player->SetBodyMode(cParam02);
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
				} break;
				case 13: // Update Skill Points 
				{
					SQLINTEGER cParam02;
					if (m_dbLink.sqlCmd.GetData(5, SQL_C_LONG, &cParam02, 0, NULL))
					{
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
							player->UpdateSP(cParam02);
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
				} break;
				case 14: // Change Guild NickName
				{
					char cParam01[13];
					if (m_dbLink.sqlCmd.GetData(4, SQL_C_CHAR, &cParam01, 13, 0))
					{
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
							player->ApplyGuildNickName(cParam01);
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
				} break;
				case 3312: // For testing references
				{
					CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
					if (player)
					{
						std::cout << " - CGObjPC ptr: " << player << std::endl;
					}
					else
						actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
				} break;
				default:
					std::cout << " Error on Action_ID (" << cActionID << ") : Undefined" << std::endl;
					actionResult = FETCH_ACTION_STATE::ACTION_UNDEFINED;
					break;
				}
			}
			catch (std::exception& ex)
			{
				std::cout << " Exception on Action_ID (" << cActionID << ") : " << ex.what() << std::endl;
				actionResult = FETCH_ACTION_STATE::UNNEXPECTED_EXCEPTION;
			}

			// Update action result from table by row id
			std::wstringstream qUpdateResult;
			qUpdateResult << "UPDATE dbo._ExeGameServer";
			qUpdateResult << " SET Action_Result = " << actionResult;
			qUpdateResult << " WHERE ID = " << cID;
			m_dbLinkHelper.sqlCmd.ExecuteQuery((SQLWCHAR*)qUpdateResult.str().c_str());
			m_dbLinkHelper.sqlCmd.Clear();
		}
		m_dbLink.sqlCmd.Clear();
		
		// Making like 10 querys per second
		Sleep(100);
	}

	// Close connection and dispose handlers
	m_dbLinkHelper.sqlConn.Close();
	m_dbLink.sqlConn.Close();

	// Stop flag
	m_IsRunningDatabaseFetch = false;
	std::cout << " - Fetching stopped!" << std::endl;

	return 0;
}

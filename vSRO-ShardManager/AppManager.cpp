#include "AppManager.h"
// Console stuffs
#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
// Utils
#include "Utils/Memory.h"
#include "Utils/SimpleIni.h"

/// Static stuffs
bool AppManager::m_IsInitialized;

void AppManager::Initialize()
{
	if (!m_IsInitialized)
	{
		m_IsInitialized = true;
		InitConfigFile();
		InitDebugConsole();
		InitPatchValues();
		InitHooks();
	}
}
void AppManager::InitConfigFile()
{
	CSimpleIniA ini;
	// Try to load it or create a new one
	if (ini.LoadFile("vSRO-ShardManager.ini") != SI_Error::SI_OK)
	{
		ini.SetSpaces(false);
		// Memory
		ini.SetLongValue("Account", "CHARACTERS_MAX", 4, "; Maximum characters per account (requires client edit and sql procedure also)");
		ini.SetLongValue("Guild", "UNION_LIMIT", 8, "; Union participants limit");
		ini.SetBoolValue("Fix", "PARTY_MATCH_1HOUR_DC", true, "; Fix disconnect when party takes more than 1 hour on party match");
		// App
		ini.SetBoolValue("App", "DEBUG_CONSOLE", true, "; Attach debug console");
		// Save it
		ini.SaveFile("vSRO-ShardManager.ini");
	}
}
void AppManager::InitDebugConsole()
{
	// Load file
	CSimpleIniA ini;
	ini.LoadFile("vSRO-ShardManager.ini");

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

}
void AppManager::InitPatchValues()
{
	std::cout << " * Initializing patches..." << std::endl;

	// Load file
	CSimpleIniA ini;
	ini.LoadFile("vSRO-ShardManager.ini");

	// buffers
	uint8_t byteValue;

	// Account
	if (ReadMemoryValue<uint8_t>(0x0040F47C + 2, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Account", "CHARACTERS_MAX", 4);
		printf(" - ACCOUNT_CHARACTERS_MAX (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x0040F47C + 2, newValue);
		WriteMemoryValue<uint8_t>(0x00429B83 + 2, newValue);
	}

	// Guild
	if (ReadMemoryValue<uint8_t>(0x00434311 + 1, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Guild", "UNION_LIMIT", 8);
		printf(" - GUILD_UNION_LIMIT (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x00434311 + 1, newValue);
	}

	// Fix
	if (ini.GetBoolValue("Fix", "PARTY_MATCH_1HOUR_DC", true))
	{
		printf(" - FIX_PARTY_MATCH_1HOUR_DC\r\n");
		WriteMemoryValue<uint16_t>(0x004505D9 + 3, 0x2002);
	}
}
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
#include "Utils/Memory.h"
#include "Utils/SimpleIni.h"

/// Static stuffs
bool AppManager::m_IsInitialized;
DatabaseLink AppManager::m_dbLink, AppManager::m_dbLinkHelper;
bool AppManager::m_IsDatabaseFetchStarted;
bool AppManager::m_IsDatabaseFetchThreadRunning;

void AppManager::Initialize()
{
	if (!m_IsInitialized)
	{
		m_IsInitialized = true;
		InitConfigFile();
		InitDebugConsole();
		InitPatchValues();
		InitHooks();
		if (InitSQLConnection())
			StartDatabaseFetch();
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
		ini.SetValue("Sql", "PASS", "12341", "; Password credential");
		ini.SetValue("Sql", "DB_SHARD", "SRO_VT_SHARD", "; Name used for the specified silkroad database");
		// Memory
		ini.SetLongValue("Server", "LEVEL_MAX", 110, "; Maximum level that can be reached on server");
		ini.SetLongValue("Job", "LEVEL_MAX", 7, "; Maximum level that can be reached on job suit");
		ini.SetLongValue("Race", "CH_TOTAL_MASTERIES", 330, "; Masteries amount Chinese race will obtain");
		ini.SetLongValue("Guild", "UNION_CHAT_PARTICIPANTS", 25, "; Union chat participants allowed by guild");
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

	// Maximum level limit
	if (ReadMemoryValue<uint8_t>(0x004E52C7 + 2, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Server", "LEVEL_MAX", 110);
		printf(" - SERVER_LEVEL_MAX (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x004E52C7 + 2, newValue); // Character
		WriteMemoryValue<uint8_t>(0x004D641B + 3, newValue); // Pet
		WriteMemoryValue<uint16_t>(0x004E5471 + 4, newValue * 4); // Exp bug fix
	}

	// Job level limit
	if (ReadMemoryValue<uint8_t>(0x0060DE69 + 3, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Job", "LEVEL_MAX", 7);
		printf(" - JOB_LEVEL_MAX. (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x0060DE69 + 3, newValue);
	}
	
	// CH Mastery
	if (ReadMemoryValue<uint32_t>(0x0059C5E6 + 1, uintValue))
	{
		uint32_t newValue = ini.GetLongValue("Race", "CH_TOTAL_MASTERIES", 330);
		printf(" - RACE_CH_TOTAL_MASTERIES (%d) -> (%d)\r\n", uintValue, newValue);
		WriteMemoryValue<uint32_t>(0x0059C5E6 + 1, newValue);
	}

	// Union chat participants per guild limit
	if (ReadMemoryValue<uint8_t>(0x005C4B42 + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Guild", "UNION_CHAT_PARTICIPANTS", 25);
		printf(" - GUILD_UNION_CHAT_PARTICIPANTS (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x005C4B42 + 4, newValue);
	}
}
bool AppManager::InitSQLConnection()
{
	// Check if the app has been started before
	CreateMutexA(0, FALSE, "Github.com/JellyBitz/vSRO-ServerAddon");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// Avoid create connection
		return false;
	}

	std::cout << " * Initializing database connection..." << std::endl;

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

	// Try to open the database connections
	if (m_dbLink.sqlConn.Open((SQLWCHAR*)connString.str().c_str()) && m_dbLink.sqlCmd.Open(m_dbLink.sqlConn)
		&& m_dbLinkHelper.sqlConn.Open((SQLWCHAR*)connString.str().c_str()) && m_dbLinkHelper.sqlCmd.Open(m_dbLinkHelper.sqlConn))
	{
		return true;
	}
	return false;
}
void AppManager::StartDatabaseFetch()
{
	if (!m_IsDatabaseFetchStarted && !m_IsDatabaseFetchThreadRunning)
	{
		// Start new thread fetching
		m_IsDatabaseFetchStarted = true;
		auto hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AppManager::DatabaseFetchThread, 0, 0, 0);
		//// Set thread as background process (below normal)
		//SetThreadPriority(hThread, -1);
	}
}
DWORD WINAPI AppManager::DatabaseFetchThread()
{
	std::cout << " * Waiting 1min before start fetching..." << std::endl;
	Sleep(60000);
	std::cout << " * Fetching started" << std::endl;
	m_IsDatabaseFetchThreadRunning = true;

	// Try to create table used to fetch
	std::wstringstream qCreateTable;
	qCreateTable << "IF OBJECT_ID(N'dbo._ExeGameServer', N'U') IS NULL";
	qCreateTable << " BEGIN";
	qCreateTable << " CREATE TABLE dbo._ExeGameServer";
	qCreateTable << " (ID INT IDENTITY(1,1) PRIMARY KEY,";
	qCreateTable << " Action_ID INT NOT NULL,";
	qCreateTable << " Action_Result SMALLINT NOT NULL DEFAULT 0,";
	qCreateTable << " CharName16 VARCHAR(64) NOT NULL,";
	qCreateTable << " Param01 VARCHAR(128),";
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
		m_IsDatabaseFetchThreadRunning = false;
		return 0;
	}
	m_dbLink.sqlCmd.Clear();

	// Stops this thread loop on interruption/exit
	signal(SIGINT, [](int) {
		m_IsDatabaseFetchStarted = false;
	});

	// Start fetching actions without result
	std::wstringstream qSelectActions;
	qSelectActions << "SELECT ID, Action_ID, CharName16, Param01, Param02, Param03, Param04, Param05, Param06, Param07, Param08";
	qSelectActions << " FROM dbo._ExeGameServer";
	qSelectActions << " WHERE Action_Result = " << FETCH_ACTION_STATE::UNKNOWN;
	while (m_IsDatabaseFetchStarted)
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
				case 3312: // For testing references
				{
					CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
					if (player)
					{
						std::cout << "CGObjPC ptr: " << player << std::endl;
					}
					else
						actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
				} break;
				default:
					std::cout << " * Error on Action_ID (" << cActionID << ") : Undefined" << std::endl;
					actionResult = FETCH_ACTION_STATE::ACTION_UNDEFINED;
					break;
				}
			}
			catch (std::exception& ex)
			{
				std::cout << " * Exception on Action_ID (" << cActionID << ") : " << ex.what() << std::endl;
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

	// Stop flags
	m_IsDatabaseFetchThreadRunning = false;
	m_IsDatabaseFetchStarted = false;
	std::cout << " * Fetching finished" << std::endl;

	return 0;
}

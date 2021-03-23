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
		InitDebugConsole();
		InitOffsetValues();
		if (InitSQLConnection())
		{
			InitAddressHooks();
			StartDatabaseFetch();
		}
	}
}
void AppManager::InitDebugConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);
}
void AppManager::InitAddressHooks()
{

}
void AppManager::InitOffsetValues()
{

}
bool AppManager::InitSQLConnection()
{
	std::cout << " * Initializing database connection..." << std::endl;
	auto connString = (SQLWCHAR*)L"DRIVER={SQL Server};SERVER=localhost, 1433;DATABASE=master;UID=sa;PWD=12341;";
	// Try to open the database connections
	if (m_dbLink.sqlConn.Open(connString) && m_dbLink.sqlCmd.Open(m_dbLink.sqlConn)
		&& m_dbLinkHelper.sqlConn.Open(connString) && m_dbLinkHelper.sqlCmd.Open(m_dbLinkHelper.sqlConn))
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
	qCreateTable << "IF OBJECT_ID(N'SRO_VT_SHARD.dbo._NotifyGameServer', N'U') IS NULL";
	qCreateTable << " BEGIN";
	qCreateTable << " CREATE TABLE SRO_VT_SHARD.dbo._NotifyGameServer";
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
	qSelectActions << " FROM SRO_VT_SHARD.dbo._NotifyGameServer";
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
					SQLINTEGER cParam02, cParam03, cParam04;
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
						{
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
						}
					}
					else
					{
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
					}
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
					{
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
					}
				} break;
				case 3: // Update Hwan level (Berserk title)
				{
					// Read params
					SQLINTEGER cParam02;
					if (m_dbLink.sqlCmd.GetData(5, SQL_C_ULONG, &cParam02, 0, NULL))
					{
						// Check player existence
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
							player->UpdateHwan(cParam02);
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
					else
					{
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
					}
				} break;
				case 4: // Move Player
				{
					// Read params
					SQLUSMALLINT cParam05, cParam06, cParam07, cParam08;
					if (m_dbLink.sqlCmd.GetData(8, SQL_C_USHORT, &cParam05, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(9, SQL_C_USHORT, &cParam06, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(10, SQL_C_USHORT, &cParam07, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(11, SQL_C_USHORT, &cParam08, 0, NULL))
					{
						// Check player existence
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
						{
							if(!player->MoveTo(cParam05, cParam06, cParam07, cParam08))
								actionResult = FETCH_ACTION_STATE::FUNCTION_ERROR;
						}
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
					else
					{
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
					}
				} break;
				case 5: // Move Player through WorldId
				{
					// Read & check params
					SQLUINTEGER cParam04;
					SQLUSMALLINT cParam05, cParam06, cParam07, cParam08;
					if ( m_dbLink.sqlCmd.GetData(7, SQL_C_ULONG, &cParam04, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(8, SQL_C_USHORT, &cParam05, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(9, SQL_C_USHORT, &cParam06, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(10, SQL_C_USHORT, &cParam07, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(11, SQL_C_USHORT, &cParam08, 0, NULL))
					{
						// Check player existence
						CGObjPC* player = CGObjManager::GetObjPCByCharName16(cCharName);
						if (player)
						{
							if(!player->MoveTo(cParam04 + 0x10000, cParam05, cParam06, cParam07, cParam08))
								actionResult = FETCH_ACTION_STATE::FUNCTION_ERROR;
						}
						else
							actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
					else
					{
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
					}					
				} break;
				case 6: // Drop item near player
				{
					// Read & check params
					char cParam01[128];
					SQLINTEGER cParam02;
					SQLUSMALLINT cParam03;
					if (m_dbLink.sqlCmd.GetData(4, SQL_C_CHAR, &cParam01, 128, 0)
						&& m_dbLink.sqlCmd.GetData(5, SQL_C_USHORT, &cParam02, 0, NULL)
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
					{
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
					}
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
					{
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
					}
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
			qUpdateResult << "UPDATE SRO_VT_SHARD.dbo._NotifyGameServer";
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

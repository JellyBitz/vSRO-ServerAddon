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
	std::cout << " * Waiting 30 seconds to start fetching..." << std::endl;
	Sleep(30000);
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
	qCreateTable << " CharName VARCHAR(64) NOT NULL,";
	qCreateTable << " Param_CodeName VARCHAR(128),";
	qCreateTable << " Param_Amount01 BIGINT,";
	qCreateTable << " Param_Amount02 BIGINT,";
	qCreateTable << " Param_Amount03 BIGINT,";
	qCreateTable << " Param_RegionID SMALLINT,";
	qCreateTable << " Param_PosX REAL,";
	qCreateTable << " Param_PosY REAL,";
	qCreateTable << " Param_PosZ REAL)";
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
	qSelectActions << "SELECT ID, Action_ID, CharName, Param_CodeName, Param_Amount01, Param_Amount02, Param_Amount03, Param_RegionID, Param_PosX, Param_PosY, Param_PosZ";
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
			// Read values
			SQLINTEGER cID;
			m_dbLink.sqlCmd.GetData(1, SQL_C_ULONG, &cID, 0, 0);
			// auto cActionID = app->m_sqlCmd.GetData<SQLINTEGER>(2, SQL_C_ULONG, 0, 0);
			SQLINTEGER cActionID;
			m_dbLink.sqlCmd.GetData(2, SQL_C_ULONG, &cActionID, 0, 0);
			SQLVARCHAR cCharName[64];
			m_dbLink.sqlCmd.GetData(3, SQL_C_CHAR, &cCharName, 64, 0);
			//SQLVARCHAR* cCharName = app->m_sqlCmd.GetDataArray<SQLVARCHAR>(3, SQL_C_CHAR, 64, 0);

			// JUST FOR TESTING
			std::cout << "cID:" << cID << std::endl;
			std::cout << "cActionID:" << cActionID << std::endl;
			std::cout << "cCharName:" << cCharName << std::endl;

			// Try to execute the action
			try {
				switch (cActionID)
				{
				case 1: // Add Item
				{
					// Read params
					SQLVARCHAR cParamCodeName[128];
					SQLINTEGER cParamAmount01, cParamAmount02, cParamAmount03;

					m_dbLink.sqlCmd.GetData(4, SQL_C_CHAR, &cParamCodeName, 128, 0);
					m_dbLink.sqlCmd.GetData(5, SQL_C_ULONG, &cParamAmount01, 0, NULL);
					m_dbLink.sqlCmd.GetData(6, SQL_C_ULONG, &cParamAmount02, 0, NULL);
					m_dbLink.sqlCmd.GetData(7, SQL_C_ULONG, &cParamAmount03, 0, NULL);

					std::stringstream charName;
					charName << cCharName;
					CGObjPC* player = CGObjManager::GetObjPCByCharName16(charName.str().c_str());
					// Check player existence
					if (player)
					{
						std::stringstream codeName;
						codeName << cParamCodeName;
						auto operationCode = player->AddItem(codeName.str().c_str(), cParamAmount01, cParamAmount02, cParamAmount03);
						std::cout << " Adding item to character [" << charName.str() << "] Result [" << operationCode << "]" << std::endl;
					}
					else
					{
						actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
					}
				} break;
				case 2: // Update Gold
				{
					// Read params
					SQLINTEGER cParamAmount01, cParamAmount02, cParamAmount03, cParamAmountTest;

					m_dbLink.sqlCmd.GetData(5, SQL_C_LONG, &cParamAmount01, 0, NULL);

					std::stringstream charName;
					charName << cCharName;
					CGObjPC* player = CGObjManager::GetObjPCByCharName16(charName.str().c_str());
					// Check player existence
					if (player)
						player->UpdateGold(cParamAmount01);
					else
						actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
				} break;
				case 3: // Update Title by index
				{
					// Read params
					SQLINTEGER cParamAmount01;
					
					m_dbLink.sqlCmd.GetData(5, SQL_C_ULONG, &cParamAmount01, 0, NULL);

					std::stringstream charName;
					charName << cCharName;
					CGObjPC* player = CGObjManager::GetObjPCByCharName16(charName.str().c_str());
					// Check player existence
					if (player)
						player->UpdateTitle(cParamAmount01);
					else
						actionResult = FETCH_ACTION_STATE::CHARNAME_NOT_FOUND;
				} break;
				default:
					std::cout << " * Error, Action_ID (" << cActionID << ") not found!" << std::endl;
					actionResult = FETCH_ACTION_STATE::ACTION_NOT_FOUND;
					break;
				}
			}
			catch (std::exception& ex) {
				std::cout << " * Exception on Action_ID (" << cActionID << ") : " << ex.what() << std::endl;
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

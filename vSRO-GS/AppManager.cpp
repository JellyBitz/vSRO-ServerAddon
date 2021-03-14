#include "AppManager.h"
// Console stuffs
#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
// Thread signal
#include <csignal>
#include <sstream>
#include <exception>

/// Static stuffs
bool AppManager::m_IsDatabaseFetchThreadRunning;
AppManager::AppManager()
{
	InitDebugConsole();
	InitOffsetValues();
	if (InitSQLConnection())
	{
		InitAddressHooks();
		StartDatabaseFetch();
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
	// Initialize database handlers
	m_dbh.sqlEnvHandle = NULL;
	m_dbh.sqlConnHandle = NULL;
	m_dbh.sqlStmtHandle = NULL;

	SQLWCHAR retconstring[SQL_RETURN_CODE_LEN];

	// Allocate handlers
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_dbh.sqlEnvHandle)) {
		std::cerr << " * Error on SQLAllocHandle - SQL_HANDLE_ENV" << std::endl;
		return false;
	}
	if (SQL_SUCCESS != SQLSetEnvAttr(m_dbh.sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0)) {
		std::cerr << " * Error on SQLSetEnvAttr" << std::endl;
		return false;
	}
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, m_dbh.sqlEnvHandle, &m_dbh.sqlConnHandle)){
		std::cerr << " * Error on SQLAllocHandle - SQL_HANDLE_DBC" << std::endl;
		return false;
	}

	// Try to connect
	SQLRETURN result = SQLDriverConnect(m_dbh.sqlConnHandle, NULL, (SQLWCHAR*)L"DRIVER={SQL Server};SERVER=localhost, 1433;DATABASE=master;UID=sa;PWD=12341;", SQL_NTS, retconstring, 1024, NULL, SQL_DRIVER_NOPROMPT);
	
	// Check results
	switch (result)
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		std::cout << " * Successfully connected to SQL Server" << std::endl;
		break;
	case SQL_INVALID_HANDLE:
	case SQL_ERROR:
		std::cerr << " * Cannot connect to SQL Server!" << std::endl;
		return false;
	default:
		printf(" * Error unnexpected connecting to SQL Server! (%d)\n",result);
		return false;
	}

	// Allocate statement handle
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, m_dbh.sqlConnHandle, &m_dbh.sqlStmtHandle)){
		std::cerr << " * Error on SQLAllocHandle - SQL_HANDLE_STMT" << std::endl;
		return false;
	}
	
	// All done successfully
	return true;
}
void AppManager::StartDatabaseFetch()
{
	if (!m_IsDatabaseFetchStarted && !m_IsDatabaseFetchThreadRunning)
	{
		// Start new thread fetching
		m_IsDatabaseFetchStarted = true;
		auto hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AppManager::DatabaseFetchThread, &m_dbh, 0, 0);
		//// Set thread as background process (below normal)
		//SetThreadPriority(hThread, -1);
	}
}
DWORD WINAPI AppManager::DatabaseFetchThread(void* Data)
{
	std::cout << " * Waiting 30 seconds to start fetching..." << std::endl;
	Sleep(30000);
	m_IsDatabaseFetchThreadRunning = true;
	// Get handler
	DatabaseHandler* dbh = (DatabaseHandler*)Data;

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

	SQLRETURN result = SQLExecDirect(dbh->sqlStmtHandle, (SQLWCHAR*)qCreateTable.str().c_str(), SQL_NTS);
	if (result != SQL_SUCCESS)
	{
		wprintf(L" * Error on Query: %s\r\n", qCreateTable.str().c_str());
		ShowError(SQL_HANDLE_STMT, dbh->sqlStmtHandle, result);
		// Stop it
		m_IsDatabaseFetchThreadRunning = false;
		Instance().m_IsDatabaseFetchStarted = false;
		return 0;
	}
	SQLFreeStmt(dbh->sqlStmtHandle, SQL_CLOSE);

	// Stops this thread loop on interruption/exit
	signal(SIGINT, [](int) {
		//Instance().m_IsDatabaseFetchStarted = false;
	});

	// Start fetching actions without result
	std::wstringstream qSelectActions;
	qSelectActions << "SELECT ID, Action_ID, CharName, Param_CodeName, Param_Amount01, Param_Amount02, Param_Amount03, Param_RegionID, Param_PosX, Param_PosY, Param_PosZ";
	qSelectActions << " FROM SRO_VT_SHARD.dbo._NotifyGameServer";
	qSelectActions << " WHERE Action_Result = 0";
	while (Instance().m_IsDatabaseFetchStarted)
	{
		// Execute query
		result = SQLExecDirect(dbh->sqlStmtHandle, (SQLWCHAR*)qSelectActions.str().c_str(), SQL_NTS);
		if (result != SQL_SUCCESS)
		{
			wprintf(L" * Error on Query: %s\r\n", qSelectActions.str().c_str());
			ShowError(SQL_HANDLE_STMT, dbh->sqlStmtHandle, result);
			break;
		}

		// Define variables used for reading result
		SQLINTEGER cID, cActionID, cResult;
		SQLVARCHAR cCharName[64], cParamCodeName[128];
		SQLBIGINT cParamAmount01, cParamAmount02, cParamAmount03;
		SQLSMALLINT cParamRegionID;
		SQLREAL cParamPosX, cParamPosY, cParamPosZ;

		// Fetch one by one
		while (SQLFetch(dbh->sqlStmtHandle) == SQL_SUCCESS)
		{
			// Read values
			SQLGetData(dbh->sqlStmtHandle, 1, SQL_C_ULONG, &cID, 0, 0);
			SQLGetData(dbh->sqlStmtHandle, 2, SQL_C_ULONG, &cActionID, 0, 0);
			SQLGetData(dbh->sqlStmtHandle, 3, SQL_C_CHAR, &cCharName, 64, 0);
			SQLGetData(dbh->sqlStmtHandle, 4, SQL_C_CHAR, &cParamCodeName, 128, 0);
			SQLGetData(dbh->sqlStmtHandle, 5, SQL_C_ULONG, &cParamAmount01, 0, 0);
			SQLGetData(dbh->sqlStmtHandle, 6, SQL_C_ULONG, &cParamAmount02, 0, 0);
			SQLGetData(dbh->sqlStmtHandle, 7, SQL_C_ULONG, &cParamAmount03, 0, 0);
			SQLGetData(dbh->sqlStmtHandle, 8, SQL_C_USHORT, &cParamRegionID, 0, 0);
			SQLGetData(dbh->sqlStmtHandle, 9, SQL_C_DOUBLE, &cParamPosX, 0, 0);
			SQLGetData(dbh->sqlStmtHandle, 10, SQL_C_DOUBLE, &cParamPosY, 0, 0);
			SQLGetData(dbh->sqlStmtHandle, 11, SQL_C_DOUBLE, &cParamPosZ, 0, 0);

			// Try to execute the action
			try {
				switch (cActionID)
				{
				default:
					std::cout << " * Error, Action_ID ("<< cActionID << ") not found!" << std::endl;
					cResult = -1;
					break;
				}
			}
			catch (std::exception& ex) {
				std::cout << " * Exception on Action_ID (" << cActionID << ") : " << ex.what() << std::endl;
			}

			// Update action result from table by row id
			std::wstringstream qUpdateResult;
			qUpdateResult << "UPDATE SRO_VT_SHARD.dbo._NotifyGameServer";
			qUpdateResult << " SET Action_Result = " << cResult;
			qUpdateResult << " WHERE ID = " << cID;
			SQLHANDLE sqlStmtHandle = NULL;
			SQLAllocHandle(SQL_HANDLE_STMT, dbh->sqlConnHandle, &sqlStmtHandle);
			result = SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)qUpdateResult.str().c_str(), SQL_NTS);
			if (result != SQL_SUCCESS)
			{
				wprintf(L" * Error on Query: %s\r\n", qUpdateResult.str().c_str());
				ShowError(SQL_HANDLE_STMT, dbh->sqlStmtHandle, result);
			}
			SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
		}
		SQLFreeStmt(dbh->sqlStmtHandle, SQL_CLOSE);

		// Making like 10 querys per second
		Sleep(100);
	}

	// Close connection and dispose handlers
	SQLFreeHandle(SQL_HANDLE_STMT, dbh->sqlStmtHandle);
	SQLDisconnect(dbh->sqlConnHandle);
	SQLFreeHandle(SQL_HANDLE_DBC, dbh->sqlConnHandle);
	SQLFreeHandle(SQL_HANDLE_ENV, dbh->sqlEnvHandle);

	std::cout << " - Stopped." << std::endl;

	// Stop flags
	m_IsDatabaseFetchThreadRunning = false;
	Instance().m_IsDatabaseFetchStarted = false;

	return 0;
}

void AppManager::ShowError(unsigned int hType, const SQLHANDLE& hHandle,SQLRETURN RetCode)
{
	if (RetCode == SQL_INVALID_HANDLE)
	{
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}

	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];

	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage, (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS)
	{
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5))
		{
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}
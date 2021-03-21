#pragma once
#include <Windows.h>
#include "Database/SQLConnection.h"
#include "Database/SQLCommand.h"

// All states fetching can generate
enum FETCH_ACTION_STATE {
	UNKNOWN = 0,
	SUCCESS = 1,
	ACTION_UNDEFINED = 2,
	UNNEXPECTED_EXCEPTION = 3,
	PARAMS_NOT_SUPPLIED = 4,
	CHARNAME_NOT_FOUND = 5,
	FUNCTION_ERROR = 6
};

// Handlers required to make a database link
struct DatabaseLink {
	SQLConnection sqlConn;
	SQLCommand sqlCmd;
};

// Application Manager sharing info to any place in the project
class AppManager
{
private: // Private members
	// Check if app has been initialized
	static bool m_IsInitialized;
	// Handlers for SQL communication
	static DatabaseLink m_dbLink, m_dbLinkHelper;
	// Check if fetch has been started
	static bool m_IsDatabaseFetchStarted;
	// Check if thread is alive
	static bool m_IsDatabaseFetchThreadRunning;
public: // Public Methods
	// Initialize manager
	static void Initialize();
private: // Private Helpers
	// Starts console if required
	static void InitDebugConsole();
	// Initialize all hooks required
	static void InitAddressHooks();
	// Set all offsets values
	static void InitOffsetValues();
	// Initialize the database communication
	static bool InitSQLConnection();
	// Starts to fetch database info
	static void StartDatabaseFetch();
	// Fetch database and execute the required actions
	static DWORD WINAPI DatabaseFetchThread();
};


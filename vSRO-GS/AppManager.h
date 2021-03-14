#pragma once
#include <Windows.h>
// SQL stuff
#define SQL_RESULT_LEN 240
#define SQL_RETURN_CODE_LEN 1000
#include <sql.h>
#include <sqlext.h>

struct DatabaseHandler {
	SQLHANDLE sqlEnvHandle;
	SQLHANDLE sqlConnHandle;
	SQLHANDLE sqlStmtHandle;
};

// Application Manager sharing info to any place in the project
class AppManager
{
	/// Singleton stuffs
private:
	// Default constructor
	AppManager();
	AppManager(AppManager const&); // Don't Implement
	void operator=(AppManager const&); // Don't Implement
public:
	static AppManager& Instance()
	{
		// Guaranteed to be destroyed and instantiated on first use.
		static AppManager _this;
		return _this;
	}
	/// Continue normally
private: // Private members
	// Handlers required to create a SQL connection
	DatabaseHandler m_dbh;
	// Check if fetch has been started
	bool m_IsDatabaseFetchStarted;
	// Check if thread is alive
	static bool m_IsDatabaseFetchThreadRunning;
private: // Private methods
	// Starts console if required
	void InitDebugConsole();
	// Initialize all hooks required
	void InitAddressHooks();
	// Set all offsets values
	void InitOffsetValues();
	// Initialize the database communication
	bool InitSQLConnection();
	// Starts to fetch database info
	void StartDatabaseFetch();
private: // Private helpers
	// Fetchs database table and execute the required actions
	static DWORD WINAPI DatabaseFetchThread(void* Data);
	// Show error result from database handler
	static void ShowError(unsigned int hType, const SQLHANDLE& hHandle, SQLRETURN RetCode);
};


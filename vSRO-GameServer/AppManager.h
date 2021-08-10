#pragma once
#include <Windows.h>
#include "Database/SQLConnection.h"
#include "Database/SQLCommand.h"
#include <cstdint>
#include <string>

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
	static DatabaseLink m_dbLink, m_dbLinkHelper, m_dbUniqueLog;
	// Flag to keep thread safe
	static bool m_IsRunningDatabaseFetch;
	// Keeps in memory the value assigned
	static std::string m_CTF_ITEM_WINNING_REWARD;
	static std::string m_CTF_ITEM_KILLING_REWARD;
	static std::string m_BA_ITEM_REWARD;
public: // Public Methods
	// Initialize manager
	static void Initialize();
private: // Private Helpers
	// Initialize the default config file if doesn't exists
	static void InitConfigFile();
	// Starts console if required
	static void InitDebugConsole();
	// Set all offsets values
	static void InitPatchValues();
	// Initialize all hooks required
	static void InitHooks();
	// Event handler for unique spawn
	static void OnUniqueSpawnMsg(uint32_t LogType, const char* Message, const char* UniqueCodeName, uint16_t RegionId, uint32_t unk01, uint32_t unk02);
	// Event handler for players killing uniques
	static void OnUniqueKilledMsg(uint32_t LogType, const char* Message, const char* UniqueCodeName, const char* CharName);
	// Starts to fetch database info
	static void InitDatabaseFetch();
	// Fetch database and execute the required actions
	static DWORD WINAPI DatabaseFetchThread();
};
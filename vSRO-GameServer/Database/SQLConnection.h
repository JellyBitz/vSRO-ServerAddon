#pragma once
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>

// SQL connection driver
class SQLConnection
{
private: /// Private Members
	// Enviroment handler
	SQLHANDLE m_EnvHandle;
	// Connection handler
	SQLHANDLE m_ConnHandle;
	// Keeps the connection state
	bool m_IsOpen;
public: /// Public Properties
	// Checks if the connection is opened
	bool IsOpen();
public: /// Constructor
	SQLConnection();
	~SQLConnection();
public: /// Public Methods
	// Open a database connection. Return success
	bool Open(SQLWCHAR* ConnectionString);
	// Close the current database connection
	void Close();
public: /// Public static helpers
	// Show the error in console
	static void ShowError(unsigned int hType, const SQLHANDLE& hHandle, SQLRETURN RetCode);
	// Returns SQL native handler from connection
	static SQLHANDLE GetConnectionHandle(SQLConnection Connection);
};
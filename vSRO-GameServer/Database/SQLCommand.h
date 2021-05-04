#pragma once
#include "SQLConnection.h"

class SQLCommand
{
private: /// Private Members
	// Statement handler
	SQLHANDLE m_StmtHandle;
	// Keeps the connection state
	bool m_IsOpen;
public: /// Public Properties
	// Check if the command has connection
	bool IsOpen();
public: /// Constructor
	SQLCommand();
	~SQLCommand();
public: /// Public Methods
	// Open a connection command. Return success
	bool Open(SQLConnection Connection);
	// Executes a query command
	bool ExecuteQuery(SQLWCHAR* CommandText);
	// Clears all info associated
	void Clear();
	// Fetch data from last query
	bool FetchData();
	// Read data from fetch
	bool GetData(SQLUSMALLINT ColumnNumber,SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLINTEGER BufferLength, SQLINTEGER* StrLen_or_IndPtr);
};
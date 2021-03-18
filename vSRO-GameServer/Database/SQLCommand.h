#pragma once
#include "SQLConnection.h"

class SQLCommand
{
private: /// Private Members
	// Statement handler
	SQLHANDLE m_StmtHandle;
	bool m_IsQueryExecuted;
public: /// Constructor
	SQLCommand();
	~SQLCommand();
public: /// Public Methods
	// Open a connection command. Return success
	bool Open(SQLConnection Connection);
	// Executes a query command
	bool ExecuteQuery(SQLWCHAR* CommandText);
	// Resets all current info
	void Clear();
	// Fetch data from last query
	bool FetchData();
	// Read data from fetch
	bool GetData(SQLUSMALLINT ColumnNumber,SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLINTEGER BufferLength, SQLINTEGER* StrLen_or_IndPtr);
	// Close the current connection command
	bool Close();
//public: // Templates
//	template <typename T>
//	T GetData(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLINTEGER BufferLength, SQLINTEGER* StrLen_or_IndPtr)
//	{
//		T targetValue;
//		ReadData(ColumnNumber, TargetType, &targetValue, BufferLength, StrLen_or_IndPtr);
//		return targetValue;
//	}
//	template <typename T>
//	T* GetDataArray(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLINTEGER BufferLength, SQLINTEGER* StrLen_or_IndPtr)
//	{
//		SQLPOINTER targetValue[BufferLength];
//		ReadData(ColumnNumber, TargetType, &targetValue, BufferLength, StrLen_or_IndPtr);
//		return targetValue;
//	}
};


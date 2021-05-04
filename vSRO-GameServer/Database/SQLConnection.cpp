#include "SQLConnection.h"
#include <iostream>

SQLConnection::SQLConnection() { }
SQLConnection::~SQLConnection() { }

bool SQLConnection::IsOpen()
{
	return m_IsOpen;
}

bool SQLConnection::Open(SQLWCHAR* ConnectionString)
{
	// Try to close last connection
	Close();

	SQLRETURN result;
	// Allocating handlers
	result = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_EnvHandle);
	if (result != SQL_SUCCESS)
	{
		ShowError(SQL_HANDLE_ENV, m_EnvHandle, result);
		return false;
	}
	result = SQLSetEnvAttr(m_EnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	if (result != SQL_SUCCESS)
	{
		ShowError(SQL_HANDLE_ENV, m_EnvHandle, result);
		return false;
	}
	result = SQLAllocHandle(SQL_HANDLE_DBC, m_EnvHandle, &m_ConnHandle);
	if (result != SQL_SUCCESS)
	{
		ShowError(SQL_HANDLE_DBC, m_ConnHandle, result);
		return false;
	}

	// Connect to database
	SQLWCHAR retConString[1000];
	result = SQLDriverConnect(m_ConnHandle, NULL, ConnectionString, SQL_NTS, retConString, 1024, NULL, SQL_DRIVER_NOPROMPT);
	
	// Check connection results
	switch (result)
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		m_IsOpen = true;
		return true;
	}
	ShowError(SQL_HANDLE_DBC, m_ConnHandle, result);
	return false;
}

void SQLConnection::Close()
{
	if (IsOpen())
	{
		// Close connection and dispose handlers
		SQLDisconnect(m_ConnHandle);
		SQLFreeHandle(SQL_HANDLE_DBC, m_ConnHandle);
		SQLFreeHandle(SQL_HANDLE_ENV, m_EnvHandle);
		m_IsOpen = false;
	}
}

void SQLConnection::ShowError(unsigned int hType, const SQLHANDLE& hHandle, SQLRETURN RetCode)
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

SQLHANDLE SQLConnection::GetConnectionHandle(SQLConnection Connection)
{
	return Connection.m_ConnHandle;
}
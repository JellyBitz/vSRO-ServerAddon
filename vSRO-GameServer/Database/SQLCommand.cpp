#include "SQLCommand.h"
#include <iostream>

SQLCommand::SQLCommand() { }
SQLCommand::~SQLCommand() { }

bool SQLCommand::Open(SQLConnection Connection)
{
	SQLRETURN result;
	result = SQLAllocHandle(SQL_HANDLE_STMT, SQLConnection::GetConnectionHandle(Connection), &m_StmtHandle);
	if (result != SQL_SUCCESS)
	{
		SQLConnection::ShowError(SQL_HANDLE_STMT,m_StmtHandle,result);
		return false;
	}
	return true;
}
bool SQLCommand::ExecuteQuery(SQLWCHAR* CommandText)
{
	SQLRETURN result = SQLExecDirect(m_StmtHandle, CommandText, SQL_NTS);
	if (result != SQL_SUCCESS)
	{
		std::wcout << "Error on ExecuteQuery: " << CommandText << std::endl;
		SQLConnection::ShowError(SQL_HANDLE_STMT, m_StmtHandle, result);
		return false;
	}
	return true;
}
void SQLCommand::Clear()
{
	SQLFreeStmt(m_StmtHandle, SQL_CLOSE);
}
bool SQLCommand::FetchData()
{
	return SQLFetch(m_StmtHandle) == SQL_SUCCESS;
}
bool SQLCommand::GetData(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLINTEGER BufferLength, SQLINTEGER* StrLen_or_IndPtr)
{
	SQLRETURN result = SQLGetData(m_StmtHandle, ColumnNumber, TargetType, TargetValue, BufferLength, StrLen_or_IndPtr);
	if (result != SQL_SUCCESS)
	{
		std::wcout << "Error on GetData (ColumnNumber:" << ColumnNumber << ")" << std::endl;
		SQLConnection::ShowError(SQL_HANDLE_STMT, m_StmtHandle, result);
		return false;
	}
	return true;
}
bool SQLCommand::Close()
{
	SQLFreeStmt(m_StmtHandle, SQL_CLOSE);
}
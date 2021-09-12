#pragma once
#include <Windows.h>
#include <algorithm>
#include <string>

// Writes bytes to a process
static BOOL WriteProcessBytes(HANDLE hProcess, DWORD destAddress, LPVOID patch, DWORD numBytes)
{
	DWORD oldProtect = 0;	// Old protection on page we are writing to
	DWORD bytesRet = 0;		// # of bytes written
	BOOL status = TRUE;		// Status of the function

	// Change page protection so we can write executable code
	if (!VirtualProtectEx(hProcess, UlongToPtr(destAddress), numBytes, PAGE_EXECUTE_READWRITE, &oldProtect))
		return FALSE;

	// Write out the data
	if (!WriteProcessMemory(hProcess, UlongToPtr(destAddress), patch, numBytes, &bytesRet))
		status = FALSE;

	// Compare written bytes to the size of the patch
	if (bytesRet != numBytes)
		status = FALSE;

	// Restore the old page protection
	if (!VirtualProtectEx(hProcess, UlongToPtr(destAddress), numBytes, oldProtect, &oldProtect))
		status = FALSE;

	// Make sure changes are made!
	if (!FlushInstructionCache(hProcess, UlongToPtr(destAddress), numBytes))
		status = FALSE;

	// Return the final status, note once we set page protection, we don't want to prematurely return
	return status;
}

// Reads bytes of a process
static BOOL ReadProcessBytes(HANDLE hProcess, DWORD destAddress, LPVOID buffer, DWORD numBytes)
{
	DWORD oldProtect = 0;	// Old protection on page we are writing to
	DWORD bytesRet = 0;		// # of bytes written
	BOOL status = TRUE;		// Status of the function

	// Change page protection so we can read bytes
	if (!VirtualProtectEx(hProcess, UlongToPtr(destAddress), numBytes, PAGE_READONLY, &oldProtect))
		return FALSE;

	// Read in the data
	if (!ReadProcessMemory(hProcess, UlongToPtr(destAddress), buffer, numBytes, &bytesRet))
		status = FALSE;

	// Compare written bytes to the size of the patch
	if (bytesRet != numBytes)
		status = FALSE;

	// Restore the old page protection
	if (!VirtualProtectEx(hProcess, UlongToPtr(destAddress), numBytes, oldProtect, &oldProtect))
		status = FALSE;

	// Return the final status, note once we set page protection, we don't want to prematurely return
	return status;
}

// Writes an object value to the address specified
template<typename T>
static BOOL WriteMemoryValue(DWORD DestAddress, T Value)
{
	// Converts the object to a byte array extracting it from memory
	BYTE bytes[sizeof(T)];
	std::copy(static_cast<const BYTE*>(static_cast<const void*>(&Value)), static_cast<const BYTE*>(static_cast<const void*>(&Value)) + sizeof(Value), bytes);
	// Just overwrite the bytes to memory
	return WriteProcessBytes(GetCurrentProcess(), DestAddress, bytes, sizeof(T));
}

// Reads an object on memory from the address specified
template<typename T>
static BOOL ReadMemoryValue(DWORD DestAddress, T& Value)
{
	static BYTE bytes[sizeof(T)];
	BOOL result = ReadProcessBytes(GetCurrentProcess(), DestAddress, bytes, sizeof(T));
	if (result)
		Value = reinterpret_cast<T&>(bytes);
	return result;
}

// Writes a string to the address specified
static BOOL WriteMemoryString(DWORD DestAddress, const char* Value)
{
	auto valueLength = strlen(Value) + 1;
	// Convert the value to char*
	char* charPtr = new char[valueLength];
	strcpy_s(charPtr, valueLength, Value);
	// Cast char* to BYTE*
	BYTE* bytes = reinterpret_cast<BYTE*>(const_cast<char*>(charPtr));
	return WriteProcessBytes(GetCurrentProcess(), DestAddress, bytes, valueLength);
}

// Reads a string on memory from the address specified
static std::string ReadMemoryString(DWORD DestAddress)
{
	// Try to read pointer to char*
	uint32_t charPtr;
	if (ReadMemoryValue<uint32_t>(DestAddress, charPtr))
		return std::string((char*)charPtr);
	// Cannot read it
	return std::string();
}

// Gets the full path from current executable
static std::string GetExecutablePath()
{
	char filename[MAX_PATH];
	HMODULE hModule = GetModuleHandle(NULL);
	return std::string(filename, GetModuleFileNameA(hModule, filename, MAX_PATH));
}

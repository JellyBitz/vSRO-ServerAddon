#pragma once
#include <Windows.h>
#include <algorithm>

template<typename T>
static auto call_virtual(void* base_ptr, unsigned index) -> T
{
	return (*static_cast<T**>(base_ptr))[index];
}

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

// Converts an object to byte array extracting it from memory
template<typename T>
static BYTE* ToByteArray(T Object)
{
	static BYTE bytes[sizeof T];
	std::copy(static_cast<const BYTE*>(static_cast<const void*>(&Object)),
		static_cast<const BYTE*>(static_cast<const void*>(&Object)) + sizeof(Object),
		bytes);
	return bytes;
}

// Writes an object value from memory to the address specified
template<typename T>
static BOOL WriteMemoryValue(DWORD DestAddress, T Value)
{
	auto bytes = ToByteArray<T>(Value);
	return WriteProcessBytes(GetCurrentProcess(), DestAddress, bytes, sizeof(T));
}

// Reads an object on memory from the address specified
template<typename T>
static BOOL ReadMemoryValue(DWORD DestAddress, T& Value)
{
	static BYTE bytes[sizeof T];
	BOOL result = ReadProcessBytes(GetCurrentProcess(), DestAddress, bytes, sizeof(T));
	if (result)
		Value = reinterpret_cast<T&>(bytes);
	return result;
}
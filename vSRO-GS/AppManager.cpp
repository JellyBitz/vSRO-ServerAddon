#include "AppManager.h"
// Console stuff
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>

AppManager::AppManager()
{
	InitDebugConsole();
	InitAddressHooks();
	InitOffsetValues();
}
void AppManager::InitDebugConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
}
void AppManager::InitAddressHooks()
{

}
void AppManager::InitOffsetValues()
{

}
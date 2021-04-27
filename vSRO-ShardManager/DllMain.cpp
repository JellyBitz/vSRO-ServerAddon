#include <Windows.h>
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)
#include "AppManager.h"

EXTERN_DLL_EXPORT BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize app
		AppManager::Initialize();
		break;
	}
	return TRUE;
}
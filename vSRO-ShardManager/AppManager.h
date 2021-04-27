#pragma once

// Application Manager sharing info to any place in the project
class AppManager
{
private: // Private members
	// Check if app has been initialized
	static bool m_IsInitialized;
public: // Public Methods
	// Initialize manager
	static void Initialize();
private: // Private Helpers
	// Starts console if required
	static void InitDebugConsole();
	// Initialize the default config file if doesn't exists
	static void InitConfigFile();
	// Initialize all hooks required
	static void InitHooks();
	// Set all offsets values
	static void InitPatchValues();
};
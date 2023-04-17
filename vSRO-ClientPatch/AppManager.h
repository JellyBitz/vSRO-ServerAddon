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
	// Set all offsets values
	static void InitPatchValues();
};
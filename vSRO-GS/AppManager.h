#pragma once

// Application Manager sharing info to any place in the project
class AppManager
{
	/// Singleton stuffs
private:
	// Default constructor
	AppManager();
	AppManager(AppManager const&); // Don't Implement
	void operator=(AppManager const&); // Don't Implement
public:
	static AppManager& Instance()
	{
		// Guaranteed to be destroyed and instantiated on first use.
		static AppManager _this;
		return _this;
	}
	/// Continue normally
private:
	// Starts console if required
	void InitDebugConsole();
	// Init hooks
	void InitAddressHooks();
	// Set all offsets values
	void InitOffsetValues();
};


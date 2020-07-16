#pragma once

constexpr char SETTINGS_LOCATION[] = "res/settings.txt";

namespace Settings 
{
	extern float fov;
	extern float sensitivity;

	extern int forward;
	extern int back;
	extern int left;
	extern int right;
		   
	extern int up;
	extern int down;

	extern int viewDistance;

	// beyanlar
	void readSettingsFile();
	void writeToSettingsFile();
}



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

	//Unchangeable Settings
	extern float aspectRatio;
	extern float ZNEAR;
	extern float ZFAR;
	extern float dig;
	extern float place;
	
	// beyanlar
	void readSettingsFile();
	void writeToSettingsFile();
}



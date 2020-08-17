#include "Settings.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include "Renderer.h"

//View
float Settings::fov = 0.0f; 
float Settings::sensitivity = 0.0f; 

//Movement
int Settings::forward = 0;
int Settings::back    = 0;
int Settings::right   = 0;
int Settings::left    = 0;

int Settings::up      = 0;
int Settings::down    = 0;

//Game
int Settings::viewDistance = 0;

//Unchangeable settings
float Settings::aspectRatio = 16.0f / 9.0f;
float Settings::ZNEAR = 0.1f;
float Settings::ZFAR = 1500.0f;

//Map
std::unordered_map<std::string, int> settingsMap =
{
	{"FOV",         0},
	{"SENSITIVITY", 1},
	{"FORWARD",     2},
	{"BACK",        3},
	{"RIGHT",       4},
	{"LEFT",        5},
	{"UP",          6},
	{"DOWN",        7},
	{"VIEWDISTANCE",8},
};

void Settings::readSettingsFile()
{
	std::ifstream stream(SETTINGS_LOCATION);

	for( std::string line; getline( stream, line ); )
	{
		// jump to other line if line starts with #
		if (line.c_str()[0] == '#' || line.find("=") == std::string::npos)
			continue;
		
		int delimeter = line.find("=") - 1;
		std::string key = line.substr(0, delimeter);
		std::string value = line.substr(delimeter + 4, line.size() - delimeter - 5);
		
		// check if key exists in map
		if(settingsMap.find(key) != settingsMap.end())
		{
			switch(settingsMap[key])
			{
				case 0:
					fov = std::stof(value);
					break;
				case 1:
					sensitivity = std::stof(value);
					break;
				case 2:
					forward = std::stoi(value);
					break;
				case 3:
					back = std::stoi(value);
					break;
				case 4:
					right = std::stoi(value);
					break;
				case 5:
					left = std::stoi(value);
					break;
				case 6:
					up = std::stoi(value);
					break;
				case 7:
					down = std::stoi(value);
					break;
				case 8:
					viewDistance = std::stoi(value);
					break;
			}
		}
	}
}

void Settings::writeToSettingsFile()
{
}


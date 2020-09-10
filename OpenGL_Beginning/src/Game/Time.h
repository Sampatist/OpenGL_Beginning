#pragma once
#include <time.h>
#include <chrono>

struct Date
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};

namespace Time
{
	void intilaize();
	void update();
	double getGameTime();
	double getDeltaGameTime();
	double getDeltaRealGameTime();
	Date getGameDate();
}
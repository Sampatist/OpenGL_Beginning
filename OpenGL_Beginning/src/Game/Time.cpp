#include "Time.h"
#include <iostream>
#include "Renderer.h"

static std::chrono::system_clock::time_point lastRecordedClock;
static std::chrono::system_clock::time_point currentTimeClock;
static std::chrono::system_clock::time_point gameStart;
static double gameRealSeconds;
static double delta;

constexpr float GAME_SECOND_RATIO = 72;

void Time::intilaize()
{
    gameStart = std::chrono::system_clock::now();
}

void Time::update()
{
    currentTimeClock = std::chrono::system_clock::now();
    delta = (currentTimeClock - lastRecordedClock).count() / 10000000.0f;
    gameRealSeconds = (currentTimeClock - gameStart).count() / 10000000.0f;
    std::cout << gameRealSeconds << std::endl;
    lastRecordedClock = currentTimeClock; 
}

double Time::getGameTime()
{
    return gameRealSeconds * GAME_SECOND_RATIO + (double)12 * (double)60 * (double)60;
}

double Time::getDeltaGameTime()
{
    return delta;
}

Date Time::getGameDate()
{
    int gameSec   = (int)(getGameTime());
    int gameMin   = (gameSec / 60);
    int gameHour  = (gameMin / 60);
    int gameDay   = (gameHour / 24);
    int gameMonth = (gameDay / 30);
    int gameYear  = (gameMonth / 12);
    return Date
    {
        gameYear,
        gameMonth % 12,
        gameDay % 30,
        gameHour % 24,
        gameMin % 60,
        gameSec % 60
    };
}
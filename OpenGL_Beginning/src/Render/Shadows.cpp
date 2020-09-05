#include "Shadows.h"
#include "Camera.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include "Time.h"

constexpr float PI = 3.14159265359f;

static double sunRadian = (Time::getGameTime() / (double)(24 * 60 * 60) * 2 * PI) - PI;

static glm::vec3 sunDir(normalize(glm::vec3(sin(sunRadian), cos(sunRadian), sin(sunRadian) * 0.4)));
static glm::vec3 sunDirDerivative(glm::vec3(cos(sunRadian), -sin(sunRadian), cos(sunRadian) * 0.4));
static glm::vec3 sunDirSecondDerivative(glm::vec3(-sin(sunRadian), -cos(sunRadian), -sin(sunRadian) * 0.4));
static glm::vec3 binormalVectorOfSun(normalize(glm::cross(sunDirDerivative, sunDirSecondDerivative)));
const float sunRadius = 0.3f;
static glm::vec3 sunDirForw(normalize(glm::vec3(sin(sunRadian + sunRadius), cos(sunRadian + sunRadius), sin(sunRadian + sunRadius) * 0.4)));
static glm::vec3 sunDirBackw(normalize(glm::vec3(sin(sunRadian - sunRadius), cos(sunRadian - sunRadius), sin(sunRadian - sunRadius) * 0.4)));


glm::vec3 Sun::GetDirection()
{
    return sunDir;
}

glm::vec3 Sun::GetDirectionForw()
{
    return sunDirForw;
}

glm::vec3 Sun::GetDirectionBackw()
{
    return sunDirBackw;
}

glm::vec3 Sun::GetBinormal()
{
    return binormalVectorOfSun;
}

static void SetDirection(double radian)
{
    sunDir = normalize(glm::vec3(sin(radian), cos(radian), sin(radian) * 0.4));
}

static void SetDirectionForw(double radian)
{
    sunDirForw = normalize(glm::vec3(sin(radian + sunRadius), cos(radian + sunRadius), sin(radian + sunRadius) * 0.4));
}

static void SetDirectionBackw(double radian)
{
    sunDirBackw = normalize(glm::vec3(sin(radian - sunRadius), cos(radian - sunRadius), sin(radian - sunRadius) * 0.4));
}

void Sun::update()
{
    sunRadian = ((Time::getGameTime() / (double)(24 * 60 * 60)) * 2 * PI) - PI;

    SetDirection(sunRadian);
    SetDirectionForw(sunRadian);
    SetDirectionBackw(sunRadian);
}

glm::mat4 Shadows::calculateSunVPMatrix()
{
    auto sunPos = sunDir * 100.0f + Camera::GetPosition();
	glm::mat4 view = glm::lookAt(sunPos, sunPos - sunDir, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::ortho(-30.0f, 10.0f, -30.0f, 30.0f, 0.1f, 600.0f);
    return proj * view;
}

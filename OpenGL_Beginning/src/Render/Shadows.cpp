#include "Shadows.h"
#include "Camera.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include "Time.h"

constexpr double PI = 3.14159265359;

static double sunRadian = (Time::getGameTime() / (double)(24 * 60 * 60) * 2 * PI) - PI;

static glm::vec<3, double, glm::packed_highp> sunDir(normalize(glm::vec3(sin(sunRadian), cos(sunRadian), sin(sunRadian) * 0.4)));
static glm::vec<3, double, glm::packed_highp> sunDirDerivative(glm::vec3(cos(sunRadian), -sin(sunRadian), cos(sunRadian) * 0.4));
static glm::vec<3, double, glm::packed_highp> sunDirSecondDerivative(glm::vec3(-sin(sunRadian), -cos(sunRadian), -sin(sunRadian) * 0.4));
static glm::vec<3, double, glm::packed_highp> binormalVectorOfSun(normalize(glm::cross(sunDirDerivative, sunDirSecondDerivative)));
const double sunRadius = 0.3f;
static glm::vec<3, double, glm::packed_highp> sunDirForw(normalize(glm::vec3(sin(sunRadian + sunRadius), cos(sunRadian + sunRadius), sin(sunRadian + sunRadius) * 0.4)));
static glm::vec<3, double, glm::packed_highp> sunDirBackw(normalize(glm::vec3(sin(sunRadian - sunRadius), cos(sunRadian - sunRadius), sin(sunRadian - sunRadius) * 0.4)));


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

glm::mat<4, 4, double, glm::packed_highp> Shadows::calculateSunVPMatrix()
{
    auto sunPos = sunDir * 100.0 + Camera::GetPosition();
	glm::mat<4, 4, double, glm::packed_highp> view = glm::lookAt(sunPos, sunPos - sunDir, glm::vec<3, double, glm::packed_highp>(0.0f, 1.0f, 0.0f));
    glm::mat<4, 4, double, glm::packed_highp> proj = glm::ortho(-30.0f, 10.0f, -30.0f, 30.0f, 0.1f, 600.0f);
    return proj * view;
}

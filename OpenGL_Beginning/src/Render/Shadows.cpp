#include "Shadows.h"
#include "Camera.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

static glm::vec3 sunDir(normalize(glm::vec3(sin(Game::getGameTime()), cos(Game::getGameTime()), sin(Game::getGameTime()) * 0.4)));
const float sunRadius = 0.3f;
static glm::vec3 sunDirForw(normalize(glm::vec3(sin(Game::getGameTime() + sunRadius), cos(Game::getGameTime() + sunRadius), sin(Game::getGameTime() + sunRadius) * 0.4)));
static glm::vec3 sunDirBackw(normalize(glm::vec3(sin(Game::getGameTime() - sunRadius), cos(Game::getGameTime() - sunRadius), sin(Game::getGameTime() - sunRadius) * 0.4)));

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

void Sun::SetDirection(float time)
{
    sunDir = normalize(glm::vec3(sin(time), cos(time), sin(time) * 0.4));
}

void Sun::SetDirectionForw(float time)
{
    sunDirForw = normalize(glm::vec3(sin(time + sunRadius), cos(time + sunRadius), sin(time + sunRadius) * 0.4));
}

void Sun::SetDirectionBackw(float time)
{
    sunDirBackw = normalize(glm::vec3(sin(time - sunRadius), cos(time - sunRadius), sin(time - sunRadius) * 0.4));
}

void Sun::SetDirections(float time)
{
    Sun::SetDirection(time);
    Sun::SetDirectionForw(time);
    Sun::SetDirectionBackw(time);
}

glm::mat4 Shadows::calculateSunVPMatrix()
{
    auto sunPos = sunDir * 100.0f + Camera::GetPosition();
	glm::mat4 view = glm::lookAt(sunPos, sunPos - sunDir, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::ortho(-30.0f, 10.0f, -30.0f, 30.0f, 0.1f, 600.0f);
    return proj * view;
}

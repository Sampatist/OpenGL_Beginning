#include "Shadows.h"
#include "Camera.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

static glm::vec3 sunDir(normalize(glm::vec3(sin(Game::getGameTime()), cos(Game::getGameTime()), sin(Game::getGameTime()) * 0.4)));

glm::vec3 Sun::GetDirection()
{
    return sunDir;
}

void Sun::SetDirection(float time)
{
    sunDir = normalize(glm::vec3(sin(time), cos(time), sin(time) * 0.4));
}

glm::mat4 Shadows::calculateSunVPMatrix()
{
    auto sunPos = sunDir * 100.0f + Camera::GetPosition();
	glm::mat4 view = glm::lookAt(sunPos, sunPos - sunDir, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::ortho(-30.0f, 10.0f, -30.0f, 30.0f, 0.1f, 600.0f);
    return proj * view;
}

#pragma once
#include "Renderer.h"
#include "Settings.h"
#include "Camera.h"
#include "glm/trigonometric.hpp"

//float verticalLength = glm::tan(Settings::fov / 2) * Settings::ZNEAR * 2;
//float horizontalWidth = verticalLength * Settings::aspectRatio;

namespace Sun
{
    glm::vec3 GetDirection();
    glm::vec3 SetDirection(glm::vec3 dir);
}

namespace Shadows
{
    glm::mat4 calculateSunVPMatrix();
}
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
    glm::vec3 GetDirectionForw();
    glm::vec3 GetDirectionBackw();
    glm::vec3 GetBinormal();
    void update();
}

namespace Shadows
{
    glm::mat<4, 4, double, glm::packed_highp> calculateSunVPMatrix();
}
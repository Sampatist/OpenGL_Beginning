#pragma once
#include "Renderer.h"

namespace input
{
    void bindMouse(GLFWwindow* window);
    void updateMouse(float* mouseX, float* mouseY);
    void addMouseBinding(float* inputV, int positiveKey);
}

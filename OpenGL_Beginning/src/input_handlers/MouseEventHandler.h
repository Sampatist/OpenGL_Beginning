#pragma once
#include "Renderer.h"

namespace input
{
    void bindMouse(GLFWwindow* window);
    void updateMouse(double* deltaMouseX, double* deltaMouseY);
    void addMouseBinding(float* inputV, int positiveKey);
}

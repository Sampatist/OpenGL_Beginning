#pragma once

namespace input
{
    void bindMouse(GLFWwindow* window);
    void updateMouse(float* mouseX, float* mouseY);
    void addMouseBinding(float* inputV, int positiveKey);
}

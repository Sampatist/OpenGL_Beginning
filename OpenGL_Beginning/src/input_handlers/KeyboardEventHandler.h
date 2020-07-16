#pragma once
#include "Renderer.h"

namespace input
{
    void bindKeyboard(GLFWwindow* window);
    void addKeyboardBinding(float* inputV, int positiveKey, int negativeKey);
    void addKeyboardBinding(float* inputV, int positiveKey);
}

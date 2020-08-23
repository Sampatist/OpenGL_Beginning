#include "Renderer.h"
#include <vector>
#include "MouseEventHandler.h"
#include <iostream>

static struct MouseBind
{
    float* inputV;
    int positiveKey;

    MouseBind(float* inputV, int positiveKey)
        : inputV(inputV)
        , positiveKey(positiveKey)
    {}
};

static std::vector<MouseBind> mouseBinds;

static double oldMouseX;
static double oldMouseY;

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	for (MouseBind& mouseBind : mouseBinds)
	{
		if (button == mouseBind.positiveKey)
		{
			if (action == GLFW_PRESS)
			{
				*(mouseBind.inputV) += 1;
			}
			else if (action == GLFW_RELEASE)
			{
				*(mouseBind.inputV) -= 1;
			}
		}
	}
}

void input::bindMouse(GLFWwindow* window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

void input::updateMouse(double* deltaMouseX, double* deltaMouseY)
{
	double mouseX;
	double mouseY;
	glfwGetCursorPos(Renderer::getWindow(), &mouseX, &mouseY);
	*deltaMouseX = mouseX - oldMouseX;
	*deltaMouseY = mouseY - oldMouseY;
	oldMouseX = mouseX;
	oldMouseY = mouseY;
}

void input::addMouseBinding(float* inputV, int positiveKey)
{
	mouseBinds.emplace_back(inputV, positiveKey);
}
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

static float* mX;
static float* mY;

static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    double deltaX = oldMouseX - xpos;
    double deltaY = oldMouseY - ypos;
    
    *mX += deltaX;
    *mY += deltaY;

    oldMouseX = xpos;
    oldMouseY = ypos;
}

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
	glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

void input::updateMouse(float* mouseX, float* mouseY)
{
    mX = mouseX;
    mY = mouseY;
}

void input::addMouseBinding(float* inputV, int positiveKey)
{
	mouseBinds.emplace_back(inputV, positiveKey);
}
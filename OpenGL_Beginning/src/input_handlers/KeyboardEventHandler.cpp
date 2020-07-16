#include "Renderer.h"
#include <iostream>
#include <vector>
#include "KeyboardEventHandler.h"

static struct KeyBind
{
	float* inputV;
	int positiveKey;
	int negativeKey;

	KeyBind(float* inputV, int positiveKey, int negativeKey)
		: inputV(inputV)
		, positiveKey(positiveKey)
		, negativeKey(negativeKey)
	{}
};

static std::vector<KeyBind> keyBinds;

static void kbCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	for (KeyBind &keyBind : keyBinds)
	{
		if (key == keyBind.positiveKey)
		{
			if (action == GLFW_PRESS)
			{
				*(keyBind.inputV) += 1;
			}
			else if (action == GLFW_RELEASE)
			{
				*(keyBind.inputV) -= 1;
			}
		}
		else if (key == keyBind.negativeKey)
		{
			if (action == GLFW_PRESS)
			{
				*(keyBind.inputV) += -1;
			}
			else if (action == GLFW_RELEASE)
			{
				*(keyBind.inputV) -= -1;
			}
		}
	}
}

void input::bindKeyboard(GLFWwindow* window)
{
	glfwSetKeyCallback(window, kbCallback);
}

void input::addKeyboardBinding(float* inputV, int positiveKey, int negativeKey)
{
	keyBinds.emplace_back(inputV, positiveKey, negativeKey);
}

void input::addKeyboardBinding(float* inputV, int positiveKey)
{
	keyBinds.emplace_back(inputV, positiveKey, 0);
}

#pragma once
#include "GLFW/glfw3.h"

namespace inputManager
{
	enum class Input
	{
		RIGHT,
		FORWARD,
		MOUSEX,
		MOUSEY,
		JUMP,
		FLY,
		CTRL,
		SHIFT,
		MOUSELEFT,
		MOUSERIGHT,
	};
	float getInput(Input input);
	void initialize(GLFWwindow* window);
}

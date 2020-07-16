#pragma once
#include "GL\glew.h"
#include "GLFW/glfw3.h"

namespace Renderer
{
	void initialize();
	GLFWwindow* const getWindow();
	void endFrame();
	void terminate();
}
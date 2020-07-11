#pragma once
#include "CameraController.h"
#include "inputs.h"

namespace game
{
	void initialize(GLFWwindow* window);
	void run();
	CameraController& getcamcont();
}

#include "game.h"
#include "inputs.h"

static CameraController camController = CameraController();

void game::initialize(GLFWwindow* window)
{
	inputManager::initialize(window);
}

void game::run()
{
	camController.update();
}

CameraController& game::getcamcont()
{
	return camController;
}
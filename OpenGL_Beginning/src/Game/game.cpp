#include "game.h"
#include "inputs.h"
#include "Settings.h"

static CameraController camController = CameraController();

void game::initialize(GLFWwindow* window)
{
	Settings::readSettingsFile();
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
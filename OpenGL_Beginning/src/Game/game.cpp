#include "game.h"
#include "inputs.h"
#include "Settings.h"
#include "Renderer.h"

static CameraController camController = CameraController();

void Game::initialize()
{
	Settings::readSettingsFile();
	inputManager::initialize(Renderer::getWindow());
}

void Game::run()
{
	camController.update();
}

CameraController& Game::getcamcont()
{
	return camController;
}
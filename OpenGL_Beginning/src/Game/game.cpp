#include "game.h"
#include "inputs.h"
#include "Settings.h"
#include "Renderer.h"

static float GameTime = 1.1;

void Game::initialize()
{
	Settings::readSettingsFile();
	Renderer::initialize();
	inputManager::initialize();
	ChunkManager::start();
}

void Game::run()
{
	GameTime += 0.0004;
	CameraController::update();
	Renderer::bufferChunks();
	Renderer::draw();
}

const float Game::getGameTime()
{
	return GameTime;
}

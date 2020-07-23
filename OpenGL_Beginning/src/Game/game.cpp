#include "game.h"
#include "inputs.h"
#include "Settings.h"
#include "Renderer.h"


void Game::initialize()
{
	Settings::readSettingsFile();
	Renderer::initialize();
	inputManager::initialize();
	ChunkManager::start();
}

void Game::run()
{
	CameraController::update();
	Renderer::bufferChunks();
	Renderer::draw();
}
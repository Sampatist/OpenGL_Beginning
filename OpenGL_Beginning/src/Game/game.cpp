#include "game.h"
#include "inputs.h"
#include "Settings.h"
#include "Renderer.h"
#include "Chunk/blockEdit.h"

static float GameTime = 1.0f;

void Game::initialize()
{
	Settings::readSettingsFile();
	Renderer::initialize();
	inputManager::initialize();
	ChunkManager::start();
}

void Game::run()
{
	GameTime += 0.0001f;
	inputManager::update();
	BlockEdit::update();
	PlayerController::update();
    ChunkManager::update();
	Renderer::bufferChunks();
	Renderer::draw();
}

const float Game::getGameTime()
{
	return GameTime;
}

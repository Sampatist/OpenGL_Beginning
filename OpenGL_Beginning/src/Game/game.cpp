#include "game.h"
#include "inputs.h"
#include "Settings.h"
#include "Renderer.h"
#include "Chunk/blockEdit.h"
#include "View.h"

static float GameTime = 0.5f;

void Game::initialize()
{
	Settings::readSettingsFile();
	ViewFrustum::initialize();
	Renderer::initialize();
	inputManager::initialize();
	ChunkManager::start();
}

void Game::run()
{
	GameTime += 0.0002f;
	inputManager::update();
	
	BlockEdit::update();
	PlayerController::update();

	ViewFrustum::update();

    ChunkManager::update();
	Renderer::bufferChunks();
	Renderer::draw();
}

const float Game::getGameTime()
{
	return GameTime;
}

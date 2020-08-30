#include "game.h"
#include "inputs.h"
#include "Settings.h"
#include "Renderer.h"
#include "Chunk/blockEdit.h"
#include "View.h"
#include "PhysicsEngine/PhysicalObjects.h"

static float GameTime = -1.8f;

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
	GameTime += 0.0005f;
	inputManager::update();
	
	BlockEdit::update();
	PlayerController::update();
	Player::updatePos();

	ViewFrustum::update();

    ChunkManager::update();
	Renderer::bufferChunks();
	Renderer::draw();
}

const float Game::getGameTime()
{
	return GameTime;
}

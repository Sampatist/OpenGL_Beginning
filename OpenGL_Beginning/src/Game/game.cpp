#include "game.h"
#include "inputs.h"
#include "Settings.h"
#include "Renderer.h"
#include "Chunk/blockEdit.h"
#include "View.h"
#include "PhysicsEngine/PhysicalObjects.h"
#include "Shadows.h"

void Game::initialize()
{
	Time::intilaize();
	Settings::readSettingsFile();
	ViewFrustum::initialize();
	Renderer::initialize();
	inputManager::initialize();
	ChunkManager::start();
}

void Game::run()
{
	Time::update();
	Sun::update();
	inputManager::update();

	BlockEdit::update();
	PlayerController::update();

	ViewFrustum::update();

    ChunkManager::update();
	Renderer::bufferChunks();
	Renderer::draw();
}
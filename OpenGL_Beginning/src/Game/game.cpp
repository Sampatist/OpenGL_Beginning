#include "game.h"
#include "inputs.h"
#include "Settings.h"
#include "Renderer.h"


void Game::initialize()
{
	Settings::readSettingsFile();
	inputManager::initialize();
	ChunkManager::start();
}

void Game::run()
{
	CameraController::update();
	//Renderer::draw();
}
/*
void Renderer::draw()
{
	chunkShader.Bind();
	std::vector<Chunk> chunks = ChunkManager::getChunks();
	for(auto& chunk : chunks)
	{
		chunk.mesh.Bind();
		glDrawElements(); 
	}
	ChunkManager::addChunkUpdate(x,z,y);
}           
*/
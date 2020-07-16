#include "Chunk.h"
#include "TerrainGenerator.h"
#include <iostream>

Chunk::Chunk(int x, int z, int y)
	: x(x), z(z), y(y), blocks({ 0 }) 
{
	blocks = TerrainGenerator::generateTerrain(x, z, y);
	//mesh = MeshGenerator::generateMesh(this);
}

Chunk::~Chunk()
{
}

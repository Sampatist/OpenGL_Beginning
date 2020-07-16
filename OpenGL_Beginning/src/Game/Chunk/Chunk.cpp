#include "Chunk.h"
#include "TerrainGenerator.h"

Chunk::Chunk(int x, int z, int y)
	: x(x), z(z), y(y), blocks({ 0 }) 
{
	blocks = TerrainGenerator::generateTerrain(x, z, y);
}

Chunk::~Chunk()
{
}

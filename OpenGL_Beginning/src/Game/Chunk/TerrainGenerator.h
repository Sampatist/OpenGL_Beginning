#pragma once
#include "Chunk.h"

namespace TerrainGenerator
{
	std::array<char, CHUNK_WIDTH * CHUNK_LENGTH * CHUNK_HEIGHT> generateTerrain(int x, int z, int y);
}

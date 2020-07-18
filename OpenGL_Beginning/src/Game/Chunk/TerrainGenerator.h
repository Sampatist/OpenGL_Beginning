#pragma once
#include "Chunk.h"

namespace TerrainGenerator
{
	std::array<uint8_t, CHUNK_WIDTH * CHUNK_LENGTH * CHUNK_HEIGHT> generateTerrain(int x, int z, int y);
}

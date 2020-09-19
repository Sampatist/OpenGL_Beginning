#pragma once
#include "Chunk.h"
#include "pairHash.h"
#include <memory>
#include <unordered_map>

namespace TerrainGenerator
{
	void generateLand(Chunk& chunk);
	void generateTree(Chunk& chunk, std::unordered_map<std::pair<int, int>, std::unique_ptr<Chunk>, hash_pair>& loadedChunksMap);
}

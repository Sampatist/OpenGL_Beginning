#pragma once
#include <vector>

namespace ChunkManager
{
	void start();
	void update();
	extern std::vector<std::vector<int32_t>> chunkMeshes;
}
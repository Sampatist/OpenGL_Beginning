#pragma once
#include <vector>
#include "MeshGenerator.h"
#include <mutex>

namespace ChunkManager
{
	void start();
	void update();
	inline std::deque<MeshGenerator::Mesh> chunkMeshes;
	inline std::mutex meshLock;
	inline std::unordered_map<std::pair<int, int>, void*, hash_pair> bufferedInfoMap;
	inline std::mutex bufferMapLock;
}
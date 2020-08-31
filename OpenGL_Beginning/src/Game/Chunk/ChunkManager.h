#pragma once
#include <vector>
#include "MeshGenerator.h"
#include <mutex>
#include <queue>

namespace ChunkManager
{
	void start();
	void update();

	struct BlockUpdate
	{
		std::pair<int, int> chunkLocation;
		int x, z, y;
		int blockID;
	};
	
	Chunk* lock_getChunk(std::pair<int, int> chunkLocation);
	void addBlockUpdate(BlockUpdate update);

	inline std::mutex loadedChunksLock;
	inline std::deque<MeshGenerator::Mesh> chunkMeshes;
	inline std::mutex meshLock;
	inline std::queue<MeshGenerator::Mesh> blockUpdateMeshes;
	inline std::unordered_map<std::pair<int, int>, void*, hash_pair> bufferedInfoMap;
	inline std::mutex bufferMapLock;
}
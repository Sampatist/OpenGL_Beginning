#pragma once
#include <unordered_map>
#include <atomic>
#include "pairHash.h"
#include "CHUNKARRAY.h"
#include "Settings.h"

struct IsTerrain
{
	std::atomic_bool loaded = false;
	std::atomic_bool unLoaded = false;
};

namespace IsTerrainManager
{
	inline std::unordered_map<std::pair<int, int>, IsTerrain, hash_pair> IsTerrainReady;
	inline void createLoadableChunks(int cameraChunkX, int cameraChunkZ) {

		int chunkCount = 0;
		int chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
		int chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;

		for (chunkCount; chunkCount < chunkCountLookup[Settings::viewDistance]; chunkCount++)
		{
			chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
			chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;
			if (IsTerrainReady.find(std::pair<int, int>(chunkX, chunkZ)) == IsTerrainReady.end())
			{
				IsTerrainReady[std::pair<int, int>(chunkX, chunkZ)].loaded = false;
				IsTerrainReady[std::pair<int, int>(chunkX, chunkZ)].unLoaded = false;
			}
			//std::cout << IsTerrainReady[std::pair<int, int>(chunkX, chunkZ)].unLoaded.load() << std::endl;
			//IsTerrainReady[std::pair<int, int>(chunkX, chunkZ)].unLoaded = false;
			//std::cout << IsTerrainReady[std::pair<int, int>(chunkX, chunkZ)].unLoaded.load() << std::endl;
			//std::cout << IsTerrainReady.size() << "\n";
			//std::cout << chunkCount << std::endl;
		}
	}
	inline void deleteUnloadedChunks()
	{
		for (auto it = IsTerrainReady.begin(); it != IsTerrainReady.end();)
		{
			if(it->second.unLoaded.load())
				it = IsTerrainReady.erase(it);
			else
				++it;
		}
	}
	inline void initilaizeIsTerrain()
	{
		int initialCameraChunkX = (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);
		int initialCameraChunkZ = (int)floor(Camera::GetPosition().z / CHUNK_LENGTH);
		std::cout << initialCameraChunkX << ", " << initialCameraChunkZ << std::endl;

		createLoadableChunks(initialCameraChunkX, initialCameraChunkZ);
	}
	inline bool isChunkCreatedAndLoaded(std::pair<int, int> chunkLocation) {
		return IsTerrainManager::IsTerrainReady.count(chunkLocation) && IsTerrainManager::IsTerrainReady.at(chunkLocation).loaded.load() && !IsTerrainManager::IsTerrainReady.at(chunkLocation).unLoaded.load();
	}
}
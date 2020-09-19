#pragma once
#include <unordered_map>
#include <atomic>
#include "pairHash.h"
#include "CHUNKARRAY.h"
#include "Settings.h"

struct IsTerrain
{
	std::atomic_bool loaded = false;
};

static bool isFar(const std::pair<int, int>& location, int x2, int z2)
{
	int relativex = location.first - x2;
	int relativez = location.second - z2;
	return (relativex * relativex + relativez * relativez) >= (Settings::viewDistance + 1) * (Settings::viewDistance + 1);
}

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
			}
		}
	}
	inline void deleteUnloadedChunks(int oldCamChunkX, int oldCamChunkZ)
	{
		std::cout << IsTerrainReady.size() << std::endl;
		for (auto it = IsTerrainReady.begin(); it != IsTerrainReady.end();)
		{
			if(isFar(it->first, oldCamChunkX, oldCamChunkZ))
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
		return IsTerrainManager::IsTerrainReady.count(chunkLocation) && IsTerrainManager::IsTerrainReady.at(chunkLocation).loaded.load();
	}
}
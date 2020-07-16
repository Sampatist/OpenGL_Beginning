#include "ChunkManager.h"
#include "Chunk.h"
#include <vector>
#include "Camera.h"
#include "Settings.h"
#include <unordered_map>
#include <iostream>
#include "CHUNKARRAY.h"

struct hash_pair {
	template <class T1, class T2>
	size_t operator()(const std::pair<T1, T2>& p) const
	{
		auto hash1 = std::hash<T1>{}(p.first);
		auto hash2 = std::hash<T2>{}(p.second);
		return hash1 ^ hash2;
	}
};

static std::vector<Chunk> loadedChunks;
static std::unordered_map<std::pair<int, int>, Chunk*, hash_pair> loadedChunksMap;

static bool isFar(int x, int z)
{
	int relativex = (floor(Camera::GetPosition().x / CHUNK_WIDTH)- x);
	int relativez = (floor(Camera::GetPosition().z / CHUNK_LENGTH) - z);
	if((relativex * relativex + relativez * relativez) >= (Settings::viewDistance + 1) * (Settings::viewDistance + 1))
	{
		return true;
	}
	return false;
}

static void reloadChunks()
{
	for(int i = loadedChunks.size() - 1; i >= 0; i--)
	{
		if(isFar(loadedChunks[i].getX(), loadedChunks[i].getZ()))
		{
			// unload chunk
			printf("unloaded chunk, x:%d z:%d y:%d\n", loadedChunks[i].getX(), loadedChunks[i].getZ(), loadedChunks[i].getY());
			std::pair<int, int> chunkLocation(loadedChunks[i].getX(), loadedChunks[i].getZ());
			loadedChunks.erase(loadedChunks.begin() + i);
			loadedChunksMap.erase(chunkLocation);
		}
	}

	int chunkCount = 0;
	int chunkX = indexLookup[chunkCount * 2] + floor(Camera::GetPosition().x / CHUNK_WIDTH);
	int chunkZ = indexLookup[chunkCount * 2 + 1] + floor(Camera::GetPosition().z / CHUNK_LENGTH);
	while (!isFar(chunkX, chunkZ))
	{

		std::pair<int, int> chunkLocation(chunkX, chunkZ);

		if(loadedChunksMap.count(chunkLocation) == 0)
		{
			// load chunk
			loadedChunks.emplace_back(chunkX, chunkZ, 0);
			loadedChunksMap[chunkLocation] = &loadedChunks.back();
			printf("loaded chunk, x:%d z:%d y:%d\n", chunkX, chunkZ, 0);
		}

		chunkCount++;
		chunkX = indexLookup[chunkCount * 2] + floor(Camera::GetPosition().x / CHUNK_WIDTH);
		chunkZ = indexLookup[chunkCount * 2 + 1] + floor(Camera::GetPosition().z / CHUNK_LENGTH);
	}
}

void ChunkManager::start()
{
	int chunkCount = 0;
	int chunkX = indexLookup[chunkCount * 2] + floor(Camera::GetPosition().x / CHUNK_WIDTH);
	int chunkZ = indexLookup[chunkCount * 2 + 1] + floor(Camera::GetPosition().z / CHUNK_LENGTH);
	while(!isFar(chunkX, chunkZ))
	{

		std::pair<int, int> chunkLocation(chunkX, chunkZ);

		// load chunk
		loadedChunks.emplace_back(chunkX, chunkZ, 0);
		printf("loaded chunk, x:%d z:%d y:%d\n", chunkX, chunkZ, 0);
		loadedChunksMap[chunkLocation] = &loadedChunks.back();

		chunkCount++;
		chunkX = indexLookup[chunkCount * 2] + floor(Camera::GetPosition().x / CHUNK_WIDTH);
		chunkZ = indexLookup[chunkCount * 2 + 1] + floor(Camera::GetPosition().z / CHUNK_LENGTH);
	}
}

void ChunkManager::update()
{
	printf("Reloading chunks.\n");
	reloadChunks();
	printf("%d loaded chunks.\n", loadedChunks.size());
}
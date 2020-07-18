#include "ChunkManager.h"
#include "Chunk.h"
#include <vector>
#include "Camera.h"
#include "Settings.h"
#include <unordered_map>
#include <iostream>
#include "CHUNKARRAY.h"
#include <future>
#include <queue>
#include "MeshGenerator.h"

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
static std::vector<std::vector<int32_t>> chunkMeshes;
static int oldCamChunkX = (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);
static int oldCamChunkZ = (int)floor(Camera::GetPosition().z / CHUNK_LENGTH);

static bool isFar(int x, int z, int x2, int z2)
{
	//int relativex = ((int)floor(Camera::GetPosition().x / CHUNK_WIDTH) - x);
	//int relativez = ((int)floor(Camera::GetPosition().z / CHUNK_LENGTH) - z);
	int relativex = x - x2;
	int relativez = z - z2;
	return (relativex * relativex + relativez * relativez) >= (Settings::viewDistance + 1) * (Settings::viewDistance + 1);
}

static std::mutex mutex;

static void reloadChunks(int cameraChunkX, int cameraChunkZ)
{
	std::lock_guard<std::mutex> lock(mutex);

	//Unload Chunks O(n)
	for(int i = loadedChunks.size() - 1; i >= 0; i--)
	{
		if(isFar(loadedChunks[i].getX(), loadedChunks[i].getZ(), cameraChunkX, cameraChunkZ))
		{
			// unload chunk
			//printf("unloaded chunk, x:%d z:%d y:%d\n", loadedChunks[i].getX(), loadedChunks[i].getZ(), loadedChunks[i].getY());
			std::pair<int, int> chunkLocation(loadedChunks[i].getX(), loadedChunks[i].getZ());
			loadedChunks.erase(loadedChunks.begin() + i);
			loadedChunksMap.erase(chunkLocation);
		}
	}
	//std::cout << std::this_thread::get_id() << " finished unload.\n";

	//Load Chunks O(n)
	int chunkCount = 0;
	int chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
	//int chunkX4Mesh = chunkX;
	int chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;
	//int chunkZ4Mesh = chunkZ;
	while (!isFar(chunkX, chunkZ, cameraChunkX, cameraChunkZ))
	{

		std::pair<int, int> chunkLocation(chunkX, chunkZ);

		if(loadedChunksMap.count(chunkLocation) == 0)
		{
			// load chunk
			loadedChunks.emplace_back(chunkX, chunkZ, 0);
			loadedChunksMap[chunkLocation] = &loadedChunks.back();
			//printf("loaded chunk, x:%d z:%d y:%d\n", chunkX, chunkZ, 0);
		}

		chunkCount++;
		chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
		chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;
	}
	//std::cout << std::this_thread::get_id() << " finished load.\n";

	//MeshGeneration

	for(int i = loadedChunks.size() - 1; i >= 0; i--)
	{
		// lock
		chunkMeshes.emplace_back(MeshGenerator::generateMesh(loadedChunks.at(i)));
	}
}

static std::queue<std::future<void>> futures;

void ChunkManager::start()
{
	futures.push(std::async(std::launch::async, reloadChunks, oldCamChunkX, oldCamChunkZ));
}

void ChunkManager::update()
{
	int cameraChunkX = (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);
	int cameraChunkZ = (int)floor(Camera::GetPosition().z / CHUNK_LENGTH);

	if((cameraChunkX != oldCamChunkX) || (cameraChunkZ != oldCamChunkZ))
	{
		oldCamChunkX = cameraChunkX;
		oldCamChunkZ = cameraChunkZ;
		printf("Reloading chunks.\n");
		futures.push(std::async(std::launch::async, reloadChunks, cameraChunkX, cameraChunkZ));
	}
	if (!futures.empty() && futures.front().wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		futures.pop();
	}

	printf("%d elements.\n", futures.size());
	printf("%d loaded chunks.\n", loadedChunks.size());
}

/*
{
	for(auto chunkUpdate : updates)
	{
		chunkUpdate.do();
	}
}
*/
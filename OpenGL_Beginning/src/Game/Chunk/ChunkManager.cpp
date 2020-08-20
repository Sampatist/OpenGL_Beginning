#include "ChunkManager.h"
#include "Chunk.h"
#include <vector>
#include "Camera.h"
#include "Settings.h"
#include <unordered_map>
#include <iostream>
#include "CHUNKARRAY.h"
#include <future>
#include <list>
#include "MeshGenerator.h"
#include <algorithm>
#include "pairHash.h"

static std::unordered_map<std::pair<int, int>, Chunk, hash_pair> loadedChunksMap;
static int oldCamChunkX = (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);
static int oldCamChunkZ = (int)floor(Camera::GetPosition().z / CHUNK_LENGTH);
static std::atomic<bool> terminateStatus = false;

static bool isFar(int x, int z, int x2, int z2)
{
	int relativex = x - x2;
	int relativez = z - z2;
	return (relativex * relativex + relativez * relativez) >= (Settings::viewDistance + 1) * (Settings::viewDistance + 1);
}

static void reloadChunks(int cameraChunkX, int cameraChunkZ)
{
	//Unload Chunks O(n)
    for(auto keyValuePair : loadedChunksMap){
		Chunk& chunk = keyValuePair.second;
        if (isFar(chunk.getX(), chunk.getZ(), cameraChunkX, cameraChunkZ))
        {
			chunk.isMeshReady = false;
            loadedChunksMap.erase(keyValuePair.first);
        }
    }

	//loadedChunksMap.erase(std::remove_if(loadedChunksMap.begin(), loadedChunksMap.end(), [=](Chunk& chunk) { return isFar(chunk.getX(), chunk.getZ(), cameraChunkX, cameraChunkZ); }), loadedChunksMap.end());

	//Load Chunks O(n)
	int chunkCount = 0;
	int chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
	int chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;
	int renderDistance = Settings::viewDistance;

	while (chunkCount < chunkCountLookup[renderDistance])
	{
		std::pair<int, int> chunkLocation(chunkX, chunkZ);

		if(loadedChunksMap.count(chunkLocation) == 0)
		{
			// load chunk
			loadedChunksMap.emplace(std::make_pair(chunkLocation, Chunk{chunkX, chunkZ, 0}));
		}
		chunkCount++;
		chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
		chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;
	}

	//Create MESH
	chunkCount = 0;
	chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
	chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;

	while(chunkCount < chunkCountLookup[renderDistance])
	{
		std::pair<int, int> chunkLocation(chunkX, chunkZ);
		Chunk& chunk = loadedChunksMap.at(chunkLocation);
		if (!chunk.isMeshReady)
        {
            MeshGenerator::Mesh mesh = MeshGenerator::generateMesh(chunk, loadedChunksMap);
            ChunkManager::meshLock.lock();
            ChunkManager::chunkMeshes.push_back(mesh);
            ChunkManager::meshLock.unlock();
            chunk.isMeshReady = true;
        }
		chunkCount++;
		chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
		chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;
	}
}

/*
struct blockUpdate
{
	int x, y, z;

}

static void issueBlockUpdate(blockUpdate)
{
	//modifty loaded chunk spesific block
	//create mesh that can be accessible by buffer
	
}
*/

static std::future<void> head;
static std::pair<int, int> queued;
static bool queueEmpty = true;

void ChunkManager::start()
{
	head = std::async(std::launch::async, reloadChunks, oldCamChunkX, oldCamChunkZ);
}

void ChunkManager::update()
{
	int cameraChunkX = (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);
	int cameraChunkZ = (int)floor(Camera::GetPosition().z / CHUNK_LENGTH);

	if((cameraChunkX != oldCamChunkX) || (cameraChunkZ != oldCamChunkZ))
	{
		oldCamChunkX = cameraChunkX;
		oldCamChunkZ = cameraChunkZ;
		queued = std::make_pair(oldCamChunkX, oldCamChunkZ);
		printf("Adding queue.\n");
		queueEmpty = false;
		terminateStatus = true;
	}
	if(head.wait_for(std::chrono::seconds(0)) == std::future_status::ready && !queueEmpty)
	{
		queueEmpty = true;
		terminateStatus = false;
		head = std::async(std::launch::async, reloadChunks, queued.first, queued.second);
	}
	printf("Ready_status %d.\n", head.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
	printf("%d loaded chunks.\n", loadedChunksMap.size());
}
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


static int oldCamChunkX = (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);
static int oldCamChunkZ = (int)floor(Camera::GetPosition().z / CHUNK_LENGTH);

static bool isFar(int x, int z, int x2, int z2)
{
	int relativex = x - x2;
	int relativez = z - z2;
	return (relativex * relativex + relativez * relativez) >= (Settings::viewDistance + 1) * (Settings::viewDistance + 1);
}

static void reloadChunks(int cameraChunkX, int cameraChunkZ)
{
	//Unload Chunks O(n)
	for (auto it = ChunkManager::loadedChunksMap.begin(); it != ChunkManager::loadedChunksMap.end();) {
		Chunk& chunk = *it->second;
        if (isFar(chunk.getX(), chunk.getZ(), cameraChunkX, cameraChunkZ))
        {
			ChunkManager::loadedChunksLock.lock();
            it = ChunkManager::loadedChunksMap.erase(it);
			ChunkManager::loadedChunksLock.unlock();
        }
		else
		{
			++it;
		}
    }

	//Load Chunks O(n)
	int chunkCount = 0;
	int chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
	int chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;
	int renderDistance = Settings::viewDistance;

	while (chunkCount < chunkCountLookup[renderDistance])
	{
		std::pair<int, int> chunkLocation(chunkX, chunkZ);

		// possible access violation
		if (ChunkManager::loadedChunksMap.count(chunkLocation) == 0)
		{
			// load chunk
			// possible access violation
			ChunkManager::loadedChunksMap[chunkLocation] = std::make_shared<Chunk>(chunkX, chunkZ, 0);
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
		Chunk& chunk = *ChunkManager::loadedChunksMap.at(chunkLocation);
		if (!chunk.isMeshReady)
        {
            MeshGenerator::Mesh mesh = MeshGenerator::generateMesh(chunk, ChunkManager::loadedChunksMap);
            chunk.isMeshReady = true;
            ChunkManager::meshLock.lock();
            ChunkManager::chunkMeshes.push_back(mesh);
            ChunkManager::meshLock.unlock();
        }
		else
		{
			ChunkManager::bufferMapLock.lock();
			bool exists = ChunkManager::bufferedInfoMap.count(chunkLocation);
			ChunkManager::bufferMapLock.unlock();
			if (!exists)
			{
				MeshGenerator::Mesh mesh = MeshGenerator::generateMesh(chunk, ChunkManager::loadedChunksMap);
				ChunkManager::meshLock.lock();
				ChunkManager::chunkMeshes.push_back(mesh);
				ChunkManager::meshLock.unlock();
			}
		}
		chunkCount++;
		chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
		chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;
	}
}

static std::future<void> head;
static std::pair<int, int> queued;
static bool queueEmpty = true;

void ChunkManager::start()
{
	head = std::async(std::launch::async, reloadChunks, oldCamChunkX, oldCamChunkZ);
}

//std::vector<std::shared_ptr<Chunk>> ChunkManager::getNearChunks()
//{
//	std::vector<std::shared_ptr<Chunk>> chunks;
//	for(int x = -1; x <= 1; x++)
//	{
//		for(int z = -1; z <= 1; z++)
//		{
//			std::pair<int, int> location(oldCamChunkX + x, oldCamChunkZ + z);
//			if(loadedChunksMap.count(location) == 1)
//			{
//				chunks.push_back(loadedChunksMap[location]);
//			}
//		}
//	}
//	return chunks;
//}

void ChunkManager::update()
{
	int cameraChunkX = (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);
	int cameraChunkZ = (int)floor(Camera::GetPosition().z / CHUNK_LENGTH);

	if((cameraChunkX != oldCamChunkX) || (cameraChunkZ != oldCamChunkZ))
	{
		oldCamChunkX = cameraChunkX;
		oldCamChunkZ = cameraChunkZ;
		
		queued = std::make_pair(oldCamChunkX, oldCamChunkZ);
		//printf("Adding queue.\n");
		queueEmpty = false;
	}
	if(head.wait_for(std::chrono::seconds(0)) == std::future_status::ready && !queueEmpty)
	{
		queueEmpty = true;
		head = std::async(std::launch::async, reloadChunks, queued.first, queued.second);
	}
	//printf("Ready_status %d.\n", head.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
	printf("%d loaded chunks.\n", loadedChunksMap.size());
}
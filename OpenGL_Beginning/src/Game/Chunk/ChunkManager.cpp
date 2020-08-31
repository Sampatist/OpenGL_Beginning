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
#include <queue>
#include <unordered_set>

static std::unordered_map<std::pair<int, int>, std::unique_ptr<Chunk>, hash_pair> loadedChunksMap;

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
	for (auto it = loadedChunksMap.begin(); it != loadedChunksMap.end();) {
		Chunk& chunk = *it->second;
        if (isFar(chunk.getX(), chunk.getZ(), cameraChunkX, cameraChunkZ))
        {
			ChunkManager::loadedChunksLock.lock();
            it = loadedChunksMap.erase(it);
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

		if (loadedChunksMap.find(chunkLocation) == loadedChunksMap.end())
		{
			// load chunk
			loadedChunksMap[chunkLocation] = std::make_unique<Chunk>(chunkX, chunkZ, 0);
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
		Chunk& chunk = *loadedChunksMap.at(chunkLocation);
		if (!chunk.isMeshReady)
        {
            MeshGenerator::Mesh mesh = MeshGenerator::generateMesh(chunk, loadedChunksMap);

            chunk.isMeshReady = true;
            ChunkManager::meshLock.lock();
            ChunkManager::chunkMeshes.push_back(mesh);
            ChunkManager::meshLock.unlock();
        }
		else
		{
			ChunkManager::bufferMapLock.lock();
			if (ChunkManager::bufferedInfoMap.find(chunkLocation) == ChunkManager::bufferedInfoMap.end())
			{
				MeshGenerator::Mesh mesh = MeshGenerator::generateMesh(chunk, loadedChunksMap);
				ChunkManager::bufferMapLock.unlock();

				ChunkManager::meshLock.lock();
				ChunkManager::chunkMeshes.push_back(mesh);
				ChunkManager::meshLock.unlock();
			}
			else
			{
				ChunkManager::bufferMapLock.unlock();
			}
		}
		chunkCount++;
		chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
		chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;
	}
}


static std::queue<ChunkManager::BlockUpdate> blockUpdates;
constexpr int BLOCK_UPDATE_LIMIT = 995000;

void ChunkManager::addBlockUpdate(BlockUpdate update)
{
	blockUpdates.push(update);
}

static void reloadUpdatedChunks()
{
	static std::unordered_set<std::pair<int, int>, hash_pair> changed;

	// consume block updates
	for(int i = 0; i < BLOCK_UPDATE_LIMIT; i++)
	{
		if (blockUpdates.empty())
			break;
		ChunkManager::BlockUpdate& update = blockUpdates.front();

		if (update.x == CHUNK_WIDTH - 1)
		{
			changed.insert({ update.chunkLocation.first + 1, update.chunkLocation.second });
		}
		else if(update.x == 0)
		{
			changed.insert({ update.chunkLocation.first - 1, update.chunkLocation.second });
		}		
		if(update.z == CHUNK_LENGTH - 1)
		{
			changed.insert({ update.chunkLocation.first, update.chunkLocation.second + 1});
		}		
		else if(update.z == 0)
		{
			changed.insert({ update.chunkLocation.first, update.chunkLocation.second - 1});
		}

		ChunkManager::loadedChunksLock.lock();
		if(loadedChunksMap.find(update.chunkLocation) != loadedChunksMap.end())
		{
			loadedChunksMap.at(update.chunkLocation)->setBlock(INDEX(update.x, update.z, update.y), update.blockID);
			changed.insert(update.chunkLocation);
		}
		ChunkManager::loadedChunksLock.unlock();

		blockUpdates.pop();
	}

	for (auto& location : changed)
	{
		ChunkManager::loadedChunksLock.lock();
		if(loadedChunksMap.find(location) != loadedChunksMap.end())
		{
			Chunk& chunk = *loadedChunksMap.at(location);    
			ChunkManager::blockUpdateMeshes.push(MeshGenerator::generateMesh(chunk, loadedChunksMap));
		}
		ChunkManager::loadedChunksLock.unlock();
	}

	changed.clear();

}

static std::future<void> head;
static std::pair<int, int> queued;
static bool queueEmpty = true;

void ChunkManager::start()
{
	head = std::async(std::launch::async, reloadChunks, oldCamChunkX, oldCamChunkZ);
}

void ChunkManager::update()
{
	reloadUpdatedChunks();

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
	static int counter = 0;
	if(counter++ == 60)
	{
		printf("%d loaded chunks.\n", loadedChunksMap.size());
		counter = 0;
	}
}

// must lock loadedChunksLock before calling
Chunk* ChunkManager::lock_getChunk(std::pair<int, int> chunkLocation)
{
	if(loadedChunksMap.find(chunkLocation) != loadedChunksMap.end())
	{
		return loadedChunksMap.at(chunkLocation).get();
	}
	return nullptr;
}



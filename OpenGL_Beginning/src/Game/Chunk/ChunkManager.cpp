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
#include "IsTerrainReady.h"
#include "Serialization/readWritendParty.h"

static std::unordered_map<std::pair<int, int>, std::unique_ptr<Chunk>, hash_pair> loadedChunksMap;

static int oldCamChunkX = (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);
static int oldCamChunkZ = (int)floor(Camera::GetPosition().z / CHUNK_LENGTH);

static bool isFar(int x, int z, int x2, int z2)
{
	int relativex = x - x2;
	int relativez = z - z2;
	return (relativex * relativex + relativez * relativez) >= (Settings::viewDistance + 1) * (Settings::viewDistance + 1);
}

static std::atomic<bool> stop = false;

static void reloadChunks(int cameraChunkX, int cameraChunkZ)
{
	//Unload Chunks O(n)
	for (auto it = loadedChunksMap.begin(); it != loadedChunksMap.end();) {
		Chunk& chunk = *it->second;
        if (isFar(chunk.getX(), chunk.getZ(), cameraChunkX, cameraChunkZ))
        {
			IsTerrainManager::IsTerrainReady.at(it->first).unLoaded.store(true);
			if (chunk.getX() == 0 && chunk.getZ() == 0)
			{
				std::unordered_map<int, std::string> chunkSaveData;
				
				char* compressed = new char[16 * 16 * 256];
				{
					int sizecompressed = LZ4_compress_default((const char*)(chunk.getBlocks()), compressed, 16 * 16 * 256, 16 * 16 * 256);
					std::string str(compressed, sizecompressed);
					chunkSaveData[0] = str;

					std::ofstream os("res/saveFiles/deneme", std::ios::binary);
					cereal::BinaryOutputArchive archive(os);
					archive(chunkSaveData);
				}
				delete[] compressed;
			}
			ChunkManager::loadedChunksLock.lock();
            it = loadedChunksMap.erase(it);
			ChunkManager::loadedChunksLock.unlock();
        }
		else
		{
			++it;
		}
    }

	int chunkCount = 0;
	int chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
	int chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;
	int renderDistance = Settings::viewDistance;

	int meshChunkCount = 0;
	int meshChunkX = indexLookup[meshChunkCount * 2] + cameraChunkX;
	int meshChunkZ = indexLookup[meshChunkCount * 2 + 1] + cameraChunkZ;

	for (size_t i = 0; i < renderDistance; i++)
	{
		while (chunkCount < chunkCountLookup[i+1])
		{
			std::pair<int, int> chunkLocation(chunkX, chunkZ);

			if (loadedChunksMap.find(chunkLocation) == loadedChunksMap.end())
			{
				// load chunk
				if (stop.load())
				{
					stop.store(false);
					return;
				}
				IsTerrainManager::IsTerrainReady.at(chunkLocation).loaded.store(true);
				ChunkManager::loadedChunksLock.lock();
				loadedChunksMap[chunkLocation] = std::make_unique<Chunk>(chunkX, chunkZ, 0);
				ChunkManager::loadedChunksLock.unlock();
			}

			chunkCount++;
			chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
			chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;
		}

		while (meshChunkCount < chunkCountLookup[i])
		{
			std::pair<int, int> chunkLocation(meshChunkX, meshChunkZ);
			Chunk& chunk = *loadedChunksMap.at(chunkLocation);
			if (stop.load())
			{
				stop.store(false);
				return;
			}
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
			meshChunkCount++;
			meshChunkX = indexLookup[meshChunkCount * 2] + cameraChunkX;
			meshChunkZ = indexLookup[meshChunkCount * 2 + 1] + cameraChunkZ;
		}
	}

	while (meshChunkCount < chunkCountLookup[renderDistance])
	{
		std::pair<int, int> chunkLocation(meshChunkX, meshChunkZ);
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
		meshChunkCount++;
		meshChunkX = indexLookup[meshChunkCount * 2] + cameraChunkX;
		meshChunkZ = indexLookup[meshChunkCount * 2 + 1] + cameraChunkZ;
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
	std::cout << oldCamChunkX << ", " << oldCamChunkZ << std::endl;
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

		IsTerrainManager::deleteUnloadedChunks();
		IsTerrainManager::createLoadableChunks(oldCamChunkX, oldCamChunkZ);
		queued = std::make_pair(oldCamChunkX, oldCamChunkZ);

		queueEmpty = false;
		stop.store(true);
	}
	if(head.wait_for(std::chrono::seconds(0)) == std::future_status::ready && !queueEmpty)
	{
		queueEmpty = true;
		stop.store(false);
		head = std::async(std::launch::async, reloadChunks, queued.first, queued.second);
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



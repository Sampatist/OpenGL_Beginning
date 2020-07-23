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
#include <algorithm>

struct hash_pair {
	template <class T1, class T2>
	size_t operator()(const std::pair<T1, T2>& p) const
	{
		auto hash1 = std::hash<T1>{}(p.first);
		auto hash2 = std::hash<T2>{}(p.second);
		return hash1 ^ hash2;
	}
};

static std::deque<Chunk> loadedChunks;
static std::unordered_map<std::pair<int, int>, Chunk*, hash_pair> loadedChunksMap;
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

static void reloadChunks(int cameraChunkX, int cameraChunkZ)
{
	//Unload Chunks O(n)
	for (int i = loadedChunks.size() - 1; i >= 0; i--)
	{
		if (isFar(loadedChunks[i].getX(), loadedChunks[i].getZ(), cameraChunkX, cameraChunkZ))
		{
			// unload chunk
			//printf("unloaded chunk, x:%d z:%d y:%d\n", loadedChunks[i].getX(), loadedChunks[i].getZ(), loadedChunks[i].getY());
			std::pair<int, int> chunkLocation(loadedChunks[i].getX(), loadedChunks[i].getZ());
			loadedChunks[i].isMeshReady = false;
			//loadedChunks.erase(loadedChunks.begin() + i);
			loadedChunksMap.erase(chunkLocation);
		}
	}
	loadedChunks.erase(std::remove_if(loadedChunks.begin(), loadedChunks.end(), [=](Chunk& chunk) { return isFar(chunk.getX(), chunk.getZ(), cameraChunkX, cameraChunkZ); }), loadedChunks.end());
	//std::cout << std::this_thread::get_id() << " finished unload.\n";

	//Load Chunks O(n)
	int chunkCount = 0;
	int chunkX = indexLookup[chunkCount * 2] + cameraChunkX;
	int chunkZ = indexLookup[chunkCount * 2 + 1] + cameraChunkZ;

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

	//bool allDone = true;

	//for(auto i = ChunkManager::chunkMeshes.begin(); i != ChunkManager::chunkMeshes.end(); i++)
	//{
	//	if(!(*i).done.load())
	//	{
	//		allDone = false;
	//		break;
	//	}
	//}
	//if(allDone)
	//{
	//	ChunkManager::chunkMeshes.clear();
	//}

	//if( std::all_of(ChunkManager::chunkMeshes.begin(), ChunkManager::chunkMeshes.end(), [](Mesh& mesh){return mesh.done.load();}) )
	//{
	//	ChunkManager::chunkMeshes.clear();
	//}

	//Create MESH
	for(size_t i = 0; i < loadedChunks.size(); i++)
	{
		Chunk& loadedChunk = loadedChunks.at(i);
		if (!loadedChunk.isMeshReady)
		{
			// emplace_back(MeshGenerator::generateMesh(loadedChunk))
			// or
			// push_back(MeshGenerator::generateMesh(loadedChunk))
			MeshGenerator::Mesh mesh = MeshGenerator::generateMesh(loadedChunk);
			ChunkManager::meshLock.lock();
			ChunkManager::chunkMeshes.push_back(mesh);
			ChunkManager::meshLock.unlock();
			loadedChunk.isMeshReady = true;
		}
	}
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
	int cameraChunkX = (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);
	int cameraChunkZ = (int)floor(Camera::GetPosition().z / CHUNK_LENGTH);

	if((cameraChunkX != oldCamChunkX) || (cameraChunkZ != oldCamChunkZ))
	{
		oldCamChunkX = cameraChunkX;
		oldCamChunkZ = cameraChunkZ;
		queued = std::make_pair(oldCamChunkX, oldCamChunkZ);
		printf("Adding queue.\n");
		queueEmpty = false;
	}
	if(head.wait_for(std::chrono::seconds(0)) == std::future_status::ready && !queueEmpty)
	{
		queueEmpty = true;
		head = std::async(std::launch::async, reloadChunks, queued.first, queued.second);
	}
	printf("Ready_status %d.\n", head.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
	printf("%d loaded chunks.\n", loadedChunks.size());
}
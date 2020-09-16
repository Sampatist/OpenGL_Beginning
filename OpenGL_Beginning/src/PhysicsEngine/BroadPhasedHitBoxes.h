#pragma once
#include <vector>
#include "Swept_AABB.h"
#include "Chunk/ChunkManager.h"
#include "Chunk/IsTerrainReady.h"

std::vector<HitBox> getBroadPhasedHitBoxes(const PhysicsObject& p)
{
	HitBox b = p.getHitBox();
	glm::vec<3, double, glm::packed_highp> v = p.getVelocity();

	std::vector<HitBox> hitboxes;

	double x = v.x > 0 ? b.x : b.x + v.x;  
	double y = v.y > 0 ? b.y : b.y + v.y;  
	double z = v.z > 0 ? b.z : b.z + v.z;

	double w = v.x > 0 ? v.x + b.w : b.w - v.x;  
	double h = v.y > 0 ? v.y + b.h : b.h - v.y;  
	double d = v.z > 0 ? v.z + b.d : b.d - v.z;  

	for(int k = floor(y); k < ceil(y + h); k++)
	{
		if (abs(double(k) - (CHUNK_HEIGHT/2.0 - 0.5)) > (CHUNK_HEIGHT/2.0 - 0.5))
		{
			continue;
		}
		for(int j = floor(z); j < ceil(z + d); j++)
		{
			for(int i = floor(x); i < ceil(x + w); i++)
			{
				int chunkX = floor(double(i) / CHUNK_WIDTH);
				int chunkZ = floor(double(j) / CHUNK_LENGTH);
				int chunkBlockX = i % CHUNK_WIDTH;
				chunkBlockX = chunkBlockX + (chunkBlockX < 0) * CHUNK_WIDTH;
				int chunkBlockZ = j % CHUNK_LENGTH;
				chunkBlockZ = chunkBlockZ + (chunkBlockZ < 0) * CHUNK_LENGTH;
				int chunkBlockY = k;
				std::pair<int, int> chunkLocation(chunkX, chunkZ);

				
 				if(IsTerrainManager::isChunkCreatedAndLoaded(chunkLocation))
				{
					ChunkManager::loadedChunksLock.lock();
					auto chunk = ChunkManager::lock_getChunk(chunkLocation);
					if(auto blockID = chunk->getBlock(chunkBlockX, chunkBlockZ, chunkBlockY))
					{
						hitboxes.push_back({ (double)i,(double)k,(double)j,1.0f,1.0f,1.0f });
					}
					ChunkManager::loadedChunksLock.unlock();
				}
			}
		}
	}
	return hitboxes;
}
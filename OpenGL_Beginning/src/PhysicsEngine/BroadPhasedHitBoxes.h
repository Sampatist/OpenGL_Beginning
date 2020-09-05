#pragma once
#include <vector>
#include "Swept_AABB.h"
#include "Chunk/ChunkManager.h"

std::vector<HitBox> getBroadPhasedHitBoxes(const PhysicsObject& p)
{
	HitBox b = p.getHitBox();
	glm::vec3 v = p.getVelocity();

	std::vector<HitBox> hitboxes;

	float x = v.x > 0 ? b.x : b.x + v.x;  
	float y = v.y > 0 ? b.y : b.y + v.y;  
	float z = v.z > 0 ? b.z : b.z + v.z;

	float w = v.x > 0 ? v.x + b.w : b.w - v.x;  
	float h = v.y > 0 ? v.y + b.h : b.h - v.y;  
	float d = v.z > 0 ? v.z + b.d : b.d - v.z;  

	for(int k = floor(y); k < ceil(y + h); k++)
	{
		if (abs(float(k) - (CHUNK_HEIGHT/2.0f - 0.5)) > (CHUNK_HEIGHT/2.0f - 0.5))
		{
			continue;
		}
		for(int j = floor(z); j < ceil(z + d); j++)
		{
			for(int i = floor(x); i < ceil(x + w); i++)
			{
				int chunkX = floor(float(i) / CHUNK_WIDTH);
				int chunkZ = floor(float(j) / CHUNK_LENGTH);
				int chunkBlockX = i % CHUNK_WIDTH;
				chunkBlockX = chunkBlockX + (chunkBlockX < 0) * CHUNK_WIDTH;
				int chunkBlockZ = j % CHUNK_LENGTH;
				chunkBlockZ = chunkBlockZ + (chunkBlockZ < 0) * CHUNK_LENGTH;
				int chunkBlockY = k;
				std::pair<int, int> chunkLocation(chunkX, chunkZ);
				ChunkManager::loadedChunksLock.lock();
				if(auto chunk = ChunkManager::lock_getChunk(chunkLocation))
				{

					if(auto blockID = chunk->getBlock(chunkBlockX, chunkBlockZ, chunkBlockY))
					{
						hitboxes.push_back({ (float)i,(float)k,(float)j,1.0f,1.0f,1.0f });
					}
				}
				ChunkManager::loadedChunksLock.unlock();
			}
		}
	}
	return hitboxes;
}
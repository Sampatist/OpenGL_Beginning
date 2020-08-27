#include "rayCast.h"

RayCast::Info RayCast::castRayAndGetTheInfoPlease(glm::vec3 pos, glm::vec3 dir, float rayLength, int limit = 30)
{
	glm::vec3 hitNormal;
	dir = glm::normalize(dir);

	int rayBlockX = std::floor(pos.x);
	int rayBlockZ = std::floor(pos.z);
	int rayBlockY = std::floor(pos.y);

	float minX = 0;
	float minZ = 0;
	float minY = 0;

	float length = 0;

	int counter = 0;

	while (counter++ < limit)
	{
		minX = 0;
		minZ = 0;
		minY = 0;
		//minX
		minX = (dir.x > 0 ? rayBlockX + 1 - pos.x : pos.x - rayBlockX) / abs(dir.x);

		//minZ
		minZ = (dir.z > 0 ? rayBlockZ + 1 - pos.z : pos.z - rayBlockZ) / abs(dir.z);

		//minY 
		minY = (dir.y > 0 ? rayBlockY + 1 - pos.y : pos.y - rayBlockY) / abs(dir.y);

		float minStep = 0;

		if (minX <= minZ && minX <= minY)
		{
			hitNormal = dir.x > 0 ? glm::vec3(-1.0f, 0.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
			rayBlockX -= (int)hitNormal.x;
			minStep = minX;
		}
		else if (minZ <= minX && minZ <= minY)
		{
			hitNormal = dir.z > 0 ? glm::vec3(0.0f, 0.0f, -1.0f) : glm::vec3(0.0f, 0.0f, 1.0f);
			rayBlockZ -= (int)hitNormal.z;
			minStep = minZ;
		}
		else
		{
			hitNormal = dir.y > 0 ? glm::vec3(0.0f, -1.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
			rayBlockY -= (int)hitNormal.y;
			minStep = minY;
		}

		glm::vec3 translate = dir * minStep;
		length += glm::length(translate);
		pos += translate;
		if(abs(pos.y - 128) > 128)
		{
			break;
		}
		if (length > rayLength)
		{
			break;
		}

		int rayChunkX = floor(float(rayBlockX) / CHUNK_WIDTH);
		int rayChunkZ = floor(float(rayBlockZ) / CHUNK_LENGTH);
		std::pair<int, int> location(rayChunkX, rayChunkZ);

		if(auto chunk = ChunkManager::lock_getChunk(location)) 
		{
			int blockChunkX = rayBlockX % CHUNK_WIDTH;
			blockChunkX = blockChunkX + (blockChunkX < 0) * CHUNK_WIDTH;
			int blockChunkZ = rayBlockZ % CHUNK_LENGTH;
			blockChunkZ = blockChunkZ + (blockChunkZ < 0) * CHUNK_LENGTH;

			if (int blockID = chunk->getBlock(blockChunkX, blockChunkZ, rayBlockY))
			{
				return { true, hitNormal, {blockID, std::pair<int, int>(rayChunkX, rayChunkZ), blockChunkX, blockChunkZ, rayBlockY} };
			}
		}
		else
		{
			break;
		}
	}
	return { false, glm::vec3(0), {0, std::pair<int, int>(0, 0), 0, 0, 0} };
}
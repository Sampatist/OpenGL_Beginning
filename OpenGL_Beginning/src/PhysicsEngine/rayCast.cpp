#include "rayCast.h"
#include "Chunk/IsTerrainReady.h"

RayCast::Info RayCast::castRayAndGetTheInfoPlease(glm::vec<3, double, glm::packed_highp> pos, glm::vec<3, double, glm::packed_highp> dir, double rayLength, int limit = 30)
{
	glm::vec<3, double, glm::packed_highp> hitNormal;
	dir = glm::normalize(dir);

	int rayBlockX = std::floor(pos.x);
	int rayBlockZ = std::floor(pos.z);
	int rayBlockY = std::floor(pos.y);

	double minX = 0;
	double minZ = 0;
	double minY = 0;

	double length = 0;

	int counter = 0;

	while (counter++ < limit)
	{
		minX = 0;
		minZ = 0;
		minY = 0;
		//minX
		minX = (dir.x > 0 ? rayBlockX + 1. - pos.x : pos.x - rayBlockX) / abs(dir.x);

		//minZ
		minZ = (dir.z > 0 ? rayBlockZ + 1. - pos.z : pos.z - rayBlockZ) / abs(dir.z);

		//minY 
		minY = (dir.y > 0 ? rayBlockY + 1. - pos.y : pos.y - rayBlockY) / abs(dir.y);

		double minStep = 0;

		if (minX <= minZ && minX <= minY)
		{
			hitNormal = dir.x > 0 ? glm::vec<3, double, glm::packed_highp>(-1.0f, 0.0f, 0.0f) : glm::vec<3, double, glm::packed_highp>(1.0f, 0.0f, 0.0f);
			rayBlockX -= (int)hitNormal.x;
			minStep = minX;
		}
		else if (minZ <= minX && minZ <= minY)
		{
			hitNormal = dir.z > 0 ? glm::vec<3, double, glm::packed_highp>(0.0f, 0.0f, -1.0f) : glm::vec<3, double, glm::packed_highp>(0.0f, 0.0f, 1.0f);
			rayBlockZ -= (int)hitNormal.z;
			minStep = minZ;
		}
		else
		{
			hitNormal = dir.y > 0 ? glm::vec<3, double, glm::packed_highp>(0.0f, -1.0f, 0.0f) : glm::vec<3, double, glm::packed_highp>(0.0f, 1.0f, 0.0f);
			rayBlockY -= (int)hitNormal.y;
			minStep = minY;
		}

		glm::vec<3, double, glm::packed_highp> translate = dir * minStep;
		length += glm::length(translate);
		pos += translate;
		if(abs(pos.y - (CHUNK_HEIGHT / 2.0 - 0.5)) > (CHUNK_HEIGHT / 2.0 - 0.5) || abs(double(rayBlockY) - (CHUNK_HEIGHT / 2 - 0.5)) > (CHUNK_HEIGHT / 2 - 0.5))
		{
			break;
		}
		if (length > rayLength)
		{
			break;
		}

		int rayChunkX = floor(double(rayBlockX) / CHUNK_WIDTH);
		int rayChunkZ = floor(double(rayBlockZ) / CHUNK_LENGTH);
		std::pair<int, int> location(rayChunkX, rayChunkZ);

		if(IsTerrainManager::isChunkCreatedAndLoaded(location))
		{
			int blockChunkX = rayBlockX % CHUNK_WIDTH;
			blockChunkX = blockChunkX + (blockChunkX < 0) * CHUNK_WIDTH;
			int blockChunkZ = rayBlockZ % CHUNK_LENGTH;
			blockChunkZ = blockChunkZ + (blockChunkZ < 0) * CHUNK_LENGTH;

			ChunkManager::loadedChunksLock.lock();
			auto chunk = ChunkManager::lock_getChunk(location);
 			if (int blockID = chunk->getBlock(blockChunkX, blockChunkZ, rayBlockY))
			{
				ChunkManager::loadedChunksLock.unlock();
				return { true, length, hitNormal, {blockID, std::pair<int, int>(rayChunkX, rayChunkZ), blockChunkX, blockChunkZ, rayBlockY} };
			}
			ChunkManager::loadedChunksLock.unlock();

		}
		else
		{
			break;
		}
	}
	return { false, 0, glm::vec<3, double, glm::packed_highp>(0), {0, std::pair<int, int>(0, 0), 0, 0, 0} };
}

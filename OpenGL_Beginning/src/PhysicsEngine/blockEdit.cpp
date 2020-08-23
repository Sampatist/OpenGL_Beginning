#include "blockEdit.h"
#include "Chunk/ChunkManager.h"
#include <math.h>
#pragma once

static BlockEdit::RayCastInfo currentInfo;

constexpr float RAY_LENGTH = 5;

BlockEdit::RayCastInfo BlockEdit::getCurrentRayInfo()
{
	return currentInfo;
}

void BlockEdit::rayCast()
{
	glm::vec3 camPos = Camera::GetPosition();
	glm::vec3 camDir = Camera::GetCameraAngle();

	FaceDirection faceDir;

	int camBlockX = std::floor(camPos.x);
	int camBlockZ = std::floor(camPos.z);
	int camBlockY = std::floor(camPos.y);

	float minX = 0;
	float minZ = 0;
	float minY = 0;

	float length = 0;

	int counter = 0;

	while(counter++ < 50)
	{
		minX = 0;
		minZ = 0;
		minY = 0;
		//minX
		minX = (camDir.x > 0 ? camBlockX + 1 - camPos.x : camPos.x - camBlockX) / abs(camDir.x);

		//minZ
		minZ = (camDir.z > 0 ? camBlockZ + 1 - camPos.z : camPos.z - camBlockZ) / abs(camDir.z);

		//minY 
		minY = (camDir.y > 0 ? camBlockY + 1 - camPos.y : camPos.y - camBlockY) / abs(camDir.y);

		float minStep = 0;

		if (minX <= minZ && minX <= minY)
		{
			faceDir = camDir.x > 0 ? FaceDirection::negX : FaceDirection::posX;
			minStep = minX;
		}
		else if(minZ <= minX && minZ <= minY)
		{
			faceDir = camDir.z > 0 ? FaceDirection::negZ : FaceDirection::posZ;
			minStep = minZ;
		}
		else
		{
			faceDir = camDir.y > 0 ? FaceDirection::negY : FaceDirection::posY;
			minStep = minY;
		}

		glm::vec3 translate = camDir * minStep;

		length += glm::length(translate);
		if(length > RAY_LENGTH)
		{
			//std::cout << "length passed limit" << std::endl;
			currentInfo = { false, (FaceDirection)0, {0, std::pair<int, int>(0, 0), 0, 0, 0} };
			break;
		}

		camPos += translate;
	
		switch(faceDir)
		{
		case FaceDirection::posX:
			camBlockX--;
			break;
		case FaceDirection::negX:
			camBlockX++;		
			break;
		case FaceDirection::posZ:
			camBlockZ--;
			break;
		case FaceDirection::negZ:
			camBlockZ++;
			break;	
		case FaceDirection::posY:
			camBlockY--;
			break;
		case FaceDirection::negY:
			camBlockY++;
			break;
		}

		int camChunkX = floor(float(camBlockX) / CHUNK_WIDTH);
		int camChunkZ = floor(float(camBlockZ) / CHUNK_LENGTH);
		std::pair<int, int> location(camChunkX, camChunkZ);

		if(ChunkManager::loadedChunksMap.count(location) == 1)
		{
			std::shared_ptr<Chunk> chunk = ChunkManager::loadedChunksMap[location];
			int chunkBlockX = camBlockX % CHUNK_WIDTH;
			chunkBlockX = chunkBlockX + (chunkBlockX < 0) * CHUNK_WIDTH;
			int chunkBlockZ = camBlockZ % CHUNK_LENGTH;
			chunkBlockZ = chunkBlockZ + (chunkBlockZ < 0) * CHUNK_LENGTH;

			if(int blockID = chunk->getBlock(chunkBlockX, chunkBlockZ, camBlockY))
			{
				//std::cout << "block detected!!!" << std::endl;
				currentInfo = { true, faceDir, {blockID, std::pair<int, int>(camChunkX, camChunkZ), chunkBlockX, chunkBlockZ, camBlockY} };
				break;
			}
		}
		else
		{
			currentInfo = { false, (FaceDirection)0, {0, std::pair<int, int>(0, 0), 0, 0, 0} };
			std::cout << "looking at unloaded chunk. wow man.\n";
			break;
		}
	}
}

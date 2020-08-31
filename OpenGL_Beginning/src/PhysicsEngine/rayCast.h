#pragma once
#include "Camera.h"
#include "Chunk/ChunkManager.h"
#include "glm/vec3.hpp"

namespace RayCast
{
	struct Info
	{
		struct Block
		{
			int blockID;
			std::pair<int, int> chunkLocation;
			int x, z, y;
		};
		bool hit;
		float rayLength;
		glm::vec3 faceNormal;
		Block block;
	};

	Info castRayAndGetTheInfoPlease(glm::vec3 pos, glm::vec3 dir, float rayLength, int limit);
}
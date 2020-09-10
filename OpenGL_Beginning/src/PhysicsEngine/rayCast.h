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
		double rayLength;
		glm::vec3 faceNormal;
		Block block;
	};

	Info castRayAndGetTheInfoPlease(glm::vec<3, double, glm::packed_highp> pos, glm::vec<3, double, glm::packed_highp> dir, double rayLength, int limit);
}
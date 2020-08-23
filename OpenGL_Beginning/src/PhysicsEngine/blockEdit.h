#pragma once
#include "Camera.h"
#include "Chunk/ChunkManager.h"

enum class FaceDirection : signed int
{
	negX = INDEX(-1, 0, 0), posX = INDEX(1, 0, 0), negZ = INDEX(0, -1, 0), posZ = INDEX(0, 1, 0), negY = INDEX(0, 0, -1), posY = INDEX(0, 0, 1)
};

namespace BlockEdit
{
	struct Block
	{
		int blockID;
		std::pair<int, int> location;
		int x, z, y;
	};
	struct RayCastInfo
	{
		bool hit;
		FaceDirection direction;
		Block block;
	};
	RayCastInfo getCurrentRayInfo();
	void rayCast();
}
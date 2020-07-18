#pragma once
#include <vector>
#include "Chunk.h"

namespace MeshGenerator
{	
	std::vector<int32_t> generateMesh(const Chunk& chunk);
}
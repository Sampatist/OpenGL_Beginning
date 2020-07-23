#pragma once
#include <vector>
#include "Chunk.h"
#include <atomic>
#include <memory>

namespace MeshGenerator
{
	struct Mesh
	{
		int x;
		int z;
		std::vector<int32_t> mesh;
		Mesh(int x, int z, std::vector<int32_t> mesh)
			: x(x), z(z), mesh(mesh)
		{};
	};
	MeshGenerator::Mesh generateMesh(const Chunk& chunk);
}
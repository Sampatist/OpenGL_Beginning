#pragma once
#include <vector>
#include "Chunk.h"
#include <atomic>
#include <memory>
#include "pairHash.h"
#include <unordered_map>
#include <deque>

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
	MeshGenerator::Mesh generateMesh(const Chunk& chunk, std::unordered_map<std::pair<int, int>, std::unique_ptr<Chunk>, hash_pair>& loadedChunksMap);
}
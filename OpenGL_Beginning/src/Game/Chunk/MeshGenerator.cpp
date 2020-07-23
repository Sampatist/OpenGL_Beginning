#include <vector>
#include "MeshGenerator.h"
#include <iostream>
#include <atomic>

enum class FaceDirection
{
	negX, posX, negZ, posZ, negY, posY
};

enum class TexCoord
{
	bottomLeft, bottomRight, topLeft, topRight
};

//                               4      4      8          8              3                  2
static inline int32_t packData(int x, int z, int y, int8_t blockID, FaceDirection dir, TexCoord texCoord)
{
	return x | (z << 4) | (y << 8) | (blockID << 16) | ((int)dir << 24) | ((int)dir << 27);
}

MeshGenerator::Mesh MeshGenerator::generateMesh(const Chunk& chunk)
{
	std::vector<int32_t> mesh;
	for(int k = 1; k < CHUNK_HEIGHT - 1; k++)
	{
		for(int j = 1; j < CHUNK_LENGTH - 1; j++)
		{
			for(int i = 1; i < CHUNK_WIDTH - 1; i++)
			{
				if(auto blockID = chunk.getBlock(i, j, k))
				{
					//posY
					if(!chunk.getBlock(i, j, k + 1))
					{
						int32_t arr[4] =
						{
							packData(0 + i, 1 + j, 1 + k, blockID, FaceDirection::posY, TexCoord::bottomLeft),
							packData(1 + i, 1 + j, 1 + k, blockID, FaceDirection::posY, TexCoord::bottomRight),
							packData(1 + i, 0 + j, 1 + k, blockID, FaceDirection::posY, TexCoord::topRight),
							packData(0 + i, 0 + j, 1 + k, blockID, FaceDirection::posY, TexCoord::topLeft)
						};
						mesh.insert(mesh.end(), std::begin(arr), std::end(arr));
					}
					//negY									
					if(!chunk.getBlock(i, j, k - 1))
					{
						int32_t arr[4] =
						{
							packData(0 + i, 0 + j, 0 + k, blockID, FaceDirection::negY, TexCoord::bottomLeft),
							packData(1 + i, 0 + j, 0 + k, blockID, FaceDirection::negY, TexCoord::bottomRight),
							packData(1 + i, 1 + j, 0 + k, blockID, FaceDirection::negY, TexCoord::topRight),
							packData(0 + i, 1 + j, 0 + k, blockID, FaceDirection::negY, TexCoord::topLeft)
						};
						mesh.insert(mesh.end(), std::begin(arr), std::end(arr));
					}
					//posZ
					if (!chunk.getBlock(i, j + 1, k))
					{
						int32_t arr[4] =
						{
							packData(0 + i, 1 + j, 0 + k, blockID, FaceDirection::posZ, TexCoord::bottomLeft),
							packData(1 + i, 1 + j, 0 + k, blockID, FaceDirection::posZ, TexCoord::bottomRight),
							packData(1 + i, 1 + j, 1 + k, blockID, FaceDirection::posZ, TexCoord::topRight),
							packData(0 + i, 1 + j, 1 + k, blockID, FaceDirection::posZ, TexCoord::topLeft)
						};
						mesh.insert(mesh.end(), std::begin(arr), std::end(arr));
					}
					//negZ
					if(!chunk.getBlock(i, j - 1, k))
					{
						int32_t arr[4] =
						{
							packData(0 + i, 0 + j, 1 + k, blockID, FaceDirection::negZ, TexCoord::bottomLeft),
							packData(1 + i, 0 + j, 1 + k, blockID, FaceDirection::negZ, TexCoord::bottomRight),
							packData(1 + i, 0 + j, 0 + k, blockID, FaceDirection::negZ, TexCoord::topRight),
							packData(0 + i, 0 + j, 0 + k, blockID, FaceDirection::negZ, TexCoord::topLeft)
						};
						mesh.insert(mesh.end(), std::begin(arr), std::end(arr));
					}
					//posX
					if(!chunk.getBlock(i + 1, j, k))
					{
						int32_t arr[4] =
						{
							packData(1 + i, 1 + j, 0 + k, blockID, FaceDirection::posX, TexCoord::bottomLeft),
							packData(1 + i, 0 + j, 0 + k, blockID, FaceDirection::posX, TexCoord::bottomRight),
							packData(1 + i, 0 + j, 1 + k, blockID, FaceDirection::posX, TexCoord::topRight),
							packData(1 + i, 1 + j, 1 + k, blockID, FaceDirection::posX, TexCoord::topLeft)
						};
						mesh.insert(mesh.end(), std::begin(arr), std::end(arr));
					}
					//negX
					if(!chunk.getBlock(i - 1, j, k))
					{
						int32_t arr[4] =
						{
							packData(0 + i, 0 + j, 0 + k, blockID, FaceDirection::negX, TexCoord::bottomLeft),
							packData(0 + i, 1 + j, 0 + k, blockID, FaceDirection::negX, TexCoord::bottomRight),
							packData(0 + i, 1 + j, 1 + k, blockID, FaceDirection::negX, TexCoord::topRight),
							packData(0 + i, 0 + j, 1 + k, blockID, FaceDirection::negX, TexCoord::topLeft)
						};
						mesh.insert(mesh.end(), std::begin(arr), std::end(arr));
					}
				}
			}
		}
	}
	return MeshGenerator::Mesh( chunk.getX(), chunk.getZ(), mesh );
}
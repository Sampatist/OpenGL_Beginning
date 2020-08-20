#include <vector>
#include "MeshGenerator.h"
#include <iostream>
#include <atomic>
#include <unordered_map>
#include "pairHash.h"

enum class FaceDirection
{
	negX, posX, negZ, posZ, negY, posY
};

enum class TexCoord
{
	bottomLeft, bottomRight, topLeft, topRight
};

//                               5      5      9          8              3                  2
static inline int32_t packData(int x, int z, int y, int8_t blockID, FaceDirection dir, TexCoord texCoord)
{
	return x | (z << 5) | (y << 10) | (blockID << 19) | ((int)dir << 27) | ((int)dir << 30);
}

MeshGenerator::Mesh MeshGenerator::generateMesh(const Chunk& chunk, std::unordered_map<std::pair<int, int>, Chunk, hash_pair>& loadedChunksMap)
{
	std::pair<int, int> posXpair(chunk.getX() + 1, chunk.getZ());
	std::pair<int, int> negXpair(chunk.getX() - 1, chunk.getZ());
	std::pair<int, int> posZpair(chunk.getX(), chunk.getZ() + 1);
	std::pair<int, int> negZpair(chunk.getX(), chunk.getZ() - 1);
	Chunk* posXChunk = nullptr;
	Chunk* negXChunk = nullptr;
	Chunk* posZChunk = nullptr;
	Chunk* negZChunk = nullptr;

	//posX
	if (loadedChunksMap.count(posXpair))
	{
		posXChunk = &loadedChunksMap.at(posXpair);
	}										    
	else
	{
		posXChunk = new Chunk(chunk.getX() + 1, chunk.getZ(), 0);
	}

	//negX
	if (loadedChunksMap.count(negXpair))
	{
		negXChunk = &loadedChunksMap.at(negXpair);
	}										    
	else
	{
		negXChunk = new Chunk(chunk.getX() - 1, chunk.getZ(), 0);
	}

	//posZ
	if (loadedChunksMap.count(posZpair))
	{
		posZChunk = &loadedChunksMap.at(posZpair);

	}										    
	else
	{
		posZChunk = new Chunk(chunk.getX(), chunk.getZ() + 1, 0);
	}

	//negZ
	if (loadedChunksMap.count(negZpair))
	{
		negZChunk = &loadedChunksMap.at(negZpair);
	}										    
	else
	{
		negZChunk = new Chunk(chunk.getX(), chunk.getZ() - 1, 0);
	}

	std::vector<int32_t> mesh;
	for (int k = 0; k < CHUNK_HEIGHT; k++)
	{
		for (int j = 0; j < CHUNK_LENGTH; j++)
		{
			for (int i = 0; i < CHUNK_WIDTH; i++)
			{
				if (auto blockID = chunk.getBlock(i, j, k))
				{
					//posY
					if (k != CHUNK_HEIGHT - 1 && !chunk.getBlock(i, j, k + 1))
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
					// negY									
					if (k != 0 && !chunk.getBlock(i, j, k - 1))
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
					// posZ
					if (j != CHUNK_LENGTH - 1 && !chunk.getBlock(i, j + 1, k))
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
					if (j == CHUNK_LENGTH - 1 && !posZChunk->getBlock(i, 0, k))
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
					if (j != 0 && !chunk.getBlock(i, j - 1, k))
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
					if (j == 0 && !negZChunk->getBlock(i, 15, k))
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
					if (i != CHUNK_WIDTH - 1 && !chunk.getBlock(i + 1, j, k))
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
					if (i == CHUNK_WIDTH - 1 && !posXChunk->getBlock(0, j, k))
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
					if (i != 0 && !chunk.getBlock(i - 1, j, k))
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
					if (i == 0 && !negXChunk->getBlock(15, j, k))
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

	if (!loadedChunksMap.count(posXpair)) 
		delete posXChunk;
	if (!loadedChunksMap.count(negXpair))
		delete negXChunk;
	if (!loadedChunksMap.count(posZpair))
		delete posZChunk;
	if (!loadedChunksMap.count(negZpair))
		delete negZChunk;

	return MeshGenerator::Mesh( chunk.getX(), chunk.getZ(), mesh );
}
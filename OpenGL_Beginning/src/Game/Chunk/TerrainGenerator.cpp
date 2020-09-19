#include "TerrainGenerator.h"
#include "Mr.Noise/SimplexNoise.h"
#include <iostream>
#include <functional> 
#include <glm/common.hpp>

#define DEBUG_PLANE_TERRAIN

//std::hash<std::string> TerrainHash;
//int HashValue = TerrainHash(SEED);

constexpr int OCTAVES = 10;		///10
constexpr int WATER_LEVEL = 100; //100
constexpr float AMPLITUDE = 15; //30
constexpr float SCALE = 0.0035f; //0.0035

static SimplexNoise noise;

void TerrainGenerator::generateLand(Chunk& chunk)
{
	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		for (int j = 0; j < CHUNK_LENGTH; j++)
		{
			float noiseValue2 = noise.fractal(OCTAVES, (chunk.getX() * CHUNK_WIDTH + i) * SCALE, (chunk.getZ() * CHUNK_LENGTH + j) * SCALE);
			float noiseValue = noise.fractal(5, (chunk.getX() * CHUNK_WIDTH + i) * 0.0035f, (chunk.getZ() * CHUNK_LENGTH + j) * 0.0035f);
			
			noiseValue *= 3;
			noiseValue += 1;
			float mask = noise.fractal(2, (chunk.getX() * CHUNK_WIDTH + i) * 0.0004f, (chunk.getZ() * CHUNK_LENGTH + j) * 0.0004f);
			float actualShit = 0;

			float dist = std::clamp((mask - 0.45f) * 20, 0.0f, 1.0f);
			const auto lerp = [](float a, float b, float f)
			{
				return a + f * (b - a);
			};
			actualShit = lerp(noiseValue2, noiseValue, dist);
			
			float heightValue = WATER_LEVEL + actualShit * AMPLITUDE;

			for (int k = 0; k < CHUNK_HEIGHT; k++)
			{
				if (k < heightValue - 10)
				{
					chunk.setBlock(INDEX(i, j, k), 3);
				}
				else if (k < heightValue - 4)
				{
					chunk.setBlock(INDEX(i, j, k), 2);
				}
				else if (k < heightValue)
				{
					chunk.setBlock(INDEX(i, j, k), 1);
					chunk.highest[i + j * CHUNK_WIDTH] = k;
				}
			}
		}
	}
}

void TerrainGenerator::generateTree(Chunk& chunk, std::unordered_map<std::pair<int, int>, std::unique_ptr<Chunk>, hash_pair>& loadedChunksMap)
{
	std::srand(chunk.getHash());
	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		for (int j = 0; j < CHUNK_LENGTH; j++)
		{
			float noiseValue = noise.fractal(OCTAVES, (chunk.getX() * CHUNK_WIDTH + i) * SCALE * 3, (chunk.getZ() * CHUNK_LENGTH + j) * SCALE * 3);
			float noiseValue2 = noise.fractal(OCTAVES, (chunk.getX() * CHUNK_WIDTH + i) * SCALE, (chunk.getZ() * CHUNK_LENGTH + j) * SCALE);
			if (noiseValue > -0.2 || noiseValue2 > 0)
			{
				continue;
			}
			if (std::rand() < 200)
			{
				int y = chunk.highest[i + j * CHUNK_WIDTH];
				int r = 0;
				for (int l = 0; l < 20; l++)
				{
					int log = l + y + 1;
					chunk.setBlock(INDEX(i, j, log), 2);

					if (l > 4)
					{
						if (l == 5 || l == 19)
							r = 1;
						else
							r = 2;
						for (int m = -r; m <= r; m++)
						{
							for (int n = -r; n <= r; n++)
							{
								int blockX = m + i + chunk.getX() * CHUNK_WIDTH;
								int blockZ = n + j + chunk.getZ() * CHUNK_LENGTH;
								int chunkX = floor(float(blockX) / CHUNK_WIDTH);
								int chunkZ = floor(float(blockZ) / CHUNK_LENGTH);
								if(chunkX != chunk.getX() || chunkZ != chunk.getZ())
								{
									if (loadedChunksMap.find({chunkX, chunkZ}) != loadedChunksMap.end())
									{
										Chunk& adjChunk = *loadedChunksMap.at({ chunkX, chunkZ });
										if (adjChunk.isMeshReady)
										{
											//std::cout << "UARRRRAAAAAAAAAAAA\n";
										}
										int chunkBlockX = blockX % CHUNK_WIDTH;
										chunkBlockX = chunkBlockX + (chunkBlockX < 0) * CHUNK_WIDTH;
										int chunkBlockZ = blockZ % CHUNK_LENGTH;
										chunkBlockZ = chunkBlockZ + (chunkBlockZ < 0) * CHUNK_LENGTH;
										int h = l + y + 1;
										adjChunk.setBlock(INDEX(chunkBlockX, chunkBlockZ, h), 2);
									}
									else
									{
										//std::cout << "NOOOOOO.1!^!'3123E12\n";
									}
								}
								else
								{
									int a = i + m;
									int b = j + n;
									int h = l + y + 1;
									chunk.setBlock(INDEX(a, b, h), 4);
								}
							}
						}
					}
				}
			}
		}
	}
}

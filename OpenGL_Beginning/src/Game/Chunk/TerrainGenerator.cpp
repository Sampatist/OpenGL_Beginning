#include "TerrainGenerator.h"
#include "Mr.Noise/SimplexNoise.h"
#include <iostream>

#include <functional> 

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
			float noiseValue = noise.fractal(OCTAVES, (chunk.getX() * CHUNK_WIDTH + i) * SCALE, (chunk.getZ() * CHUNK_LENGTH + j) * SCALE);
			float heightValue = WATER_LEVEL + noiseValue * AMPLITUDE;

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
				}
			}
		}
	}
}

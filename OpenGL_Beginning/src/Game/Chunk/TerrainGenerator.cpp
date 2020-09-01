#include "TerrainGenerator.h"
#include "Mr.Noise/SimplexNoise.h"
#include <iostream>

#include <functional> 

#define DEBUG_PLANE_TERRAIN

//std::hash<std::string> TerrainHash;
//int HashValue = TerrainHash(SEED);

constexpr int OCTAVES = 10;		///10
constexpr int WATER_LEVEL = 100; //100
constexpr float AMPLITUDE = 30; //30
constexpr float SCALE = 0.0035f; //0.0035

static SimplexNoise noise;

std::array<uint8_t, CHUNK_WIDTH* CHUNK_LENGTH* CHUNK_HEIGHT> TerrainGenerator::generateTerrain(int x, int z, int y)
{
	std::array<uint8_t, CHUNK_WIDTH* CHUNK_LENGTH* CHUNK_HEIGHT> blocks{ 0 };
	//for (int i = 0; i < CHUNK_WIDTH; i++)
	//{
	//	for (int j = 0; j < CHUNK_LENGTH; j++)
	//	{
	//		for (int k = 0; k < CHUNK_HEIGHT; k++)
	//		{
	//			float noiseValue = noise.fractal(OCTAVES, (x * CHUNK_WIDTH + i) * SCALE, (z * CHUNK_LENGTH + j) * SCALE, k * SCALE);
	//			if(noiseValue > 0)
	//			{
	//				blocks[INDEX(i, j, k)] = 1;
	//			}
	//		}
	//	}
	//}
	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		for (int j = 0; j < CHUNK_LENGTH; j++)
		{
			float noiseValue = noise.fractal(OCTAVES, (x * CHUNK_WIDTH + i) * SCALE, (z * CHUNK_LENGTH + j) * SCALE);
			float noiseValue2 = noise.fractal(OCTAVES - 4, (x * CHUNK_WIDTH + i) * SCALE * 4, (z * CHUNK_LENGTH + j) * SCALE / 8);
			float heightValue = WATER_LEVEL + noiseValue * AMPLITUDE;
			for (int k = 0; k < CHUNK_HEIGHT; k++)
			{
				if(k < heightValue - 10)
				{
					blocks[INDEX(i, j, k)] = 3;
				}
				else if(k < heightValue - 4)
				{
					blocks[INDEX(i, j, k)] = 2;
				}
				else if (k < heightValue)
				{
					blocks[INDEX(i, j, k)] = 1;
				}
				if (noiseValue > 0.05f && noiseValue < 0.08f && noiseValue2 < 0.1f && noiseValue2 > 0.0f && k < heightValue-8 && k > heightValue - 15)
					blocks[INDEX(i, j, k)] = 4;
			}
		}
	}
	return blocks;
}
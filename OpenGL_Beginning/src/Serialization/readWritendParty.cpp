#include "readWritendParty.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <chrono>
#include <string>
#include <array>
#include "Lz4/lz4.h"
#include "Chunk/Chunk.h"
#include "cereal/cereal.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/string.hpp"
#include <cereal/archives/binary.hpp>
#include <memory>
#include "Camera.h"

#define SAVEFILEINDEX(X,Z) (X + 32 * Z)


void Serialize::writendreadTests()
{
	std::unordered_map<int, std::string> chunkSaveData;

	char arr[16 * 16 * 256] = {65};

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			int heightValue = ((rand() % 16) * (rand() % 16)) % 60 + 100;
			for (int k = 0; k < 256; k++)
			{
				if (k < heightValue - 10)
				{
					arr[INDEX(i, j, k)] = 65;
				}
				else if (k < heightValue - 4)
				{
					arr[INDEX(i, j, k)] = 66;
				}
				else if (k < heightValue)
				{
					arr[INDEX(i, j, k)] = 67;
				}
			}
		}
	}
	
	static char* compressed = new char[16*16*256];
	auto start = std::chrono::system_clock::now();
	{
		int sizecompressed = LZ4_compress_default(arr, compressed, 16*16*256, 16*16*256);
		chunkSaveData[0] = compressed;

		std::ofstream os("res/saveFiles/deneme", std::ios::binary);
		cereal::BinaryOutputArchive archive(os);
		archive(chunkSaveData);
	}

	{
		std::unordered_map<int, std::string> chunkReadData;

		std::ifstream is("res/saveFiles/deneme", std::ios::binary);
		cereal::BinaryInputArchive archive(is);
		archive(chunkReadData);

		int sizedecompressed = LZ4_decompress_safe(chunkReadData[0].data(), arr, chunkReadData[0].size(), 16*16*256);
	}

	std::cout << (std::chrono::system_clock::now() - start).count() / 10000000.0 << std::endl;
	delete[] compressed;
}
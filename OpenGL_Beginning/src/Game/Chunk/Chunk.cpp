#include "Chunk.h"
#include "TerrainGenerator.h"
#include <iostream>
#include <algorithm>

#include "Serialization/readWritendParty.h"

Chunk::Chunk(int x, int z, int y)
	: x(x), z(z), y(y), blocks({ 0 }) 
{
	//if(x == 0 && z == 0)
	//{
	//	std::unordered_map<int, std::string> chunkReadData;
	//
	//	std::ifstream is("res/saveFiles/deneme", std::ios::binary);
	//	cereal::BinaryInputArchive archive(is);
	//	archive(chunkReadData);
	//
	//	char* arr = new char[16*16*256];
	//	int sizedecompressed = LZ4_decompress_safe(chunkReadData[0].data(), arr, chunkReadData[0].size(), 16 * 16 * 256);
	//	if(sizedecompressed == 16 * 16 * 256)
	//	{
	//		std::copy(arr, arr + sizedecompressed, blocks.data());
	//	}
	//	delete[] arr;
	//}
}

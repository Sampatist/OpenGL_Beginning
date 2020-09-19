#include "Chunk.h"
#include "TerrainGenerator.h"
#include <iostream>
#include <algorithm>

#include "Serialization/readWritendParty.h"

size_t Chunk::getHash()
{
	return hash;
}

Chunk::Chunk(int x, int z, int y)
	: x(x), z(z), y(y), treeReady(false), blocks({ 0 }), highest({ 0 })
{
	auto p_first = std::to_string(x);
	auto p_second = std::to_string(z);

	hash = std::hash<std::string>{}(p_first + ":" + p_second);
	//if(x == 0 && z == 0)
	//{
	//	std::unordered_map<int, std::string> chunkReadData;

	//	std::ifstream is("res/saveFiles/deneme", std::ios::binary);
	//	cereal::BinaryInputArchive archive(is);
	//	archive(chunkReadData);

	//	char* arr = new char[16*16*256];
	//	int sizedecompressed = LZ4_decompress_safe(chunkReadData[0].data(), arr, chunkReadData[0].size(), 16 * 16 * 256);
	//	if(sizedecompressed == 16 * 16 * 256)
	//	{
	//		std::copy(arr, arr + sizedecompressed, blocks.data());
	//	}
	//	delete[] arr;
	//}
}

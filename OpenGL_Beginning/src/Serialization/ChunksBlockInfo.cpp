#include "ChunksBlockInfo.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include "Lz4/lz4.h"
#include "Chunk/Chunk.h"
#include "cereal/cereal.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/unordered_set.hpp"
#include "cereal/types/utility.hpp"
#include "cereal/types/string.hpp"
#include <cereal/archives/binary.hpp>
#include "Camera.h"
#include "Chunk/pairHash.h"

constexpr auto SAVE_FOLDER = "res/saveFiles/";
constexpr auto REGION_FILE = "res/saveFiles/SavedChunksLocationSetsMap";

static std::unordered_map<std::pair<int, int>, std::unordered_set<int>, hash_pair> SavedChunksLocationSetsMap;

static bool doesFileExists(const char* filename)
{
	std::ifstream f(filename);
	return f.good();
}

void Serialize::initialize()
{
	if (!doesFileExists(REGION_FILE))
	{
		std::ofstream os(REGION_FILE, std::ios::binary);
		cereal::BinaryOutputArchive archive(os);
		archive(SavedChunksLocationSetsMap);
	}
	else
	{
		std::ifstream is(REGION_FILE, std::ios::binary);
		cereal::BinaryInputArchive archive(is);
		archive(SavedChunksLocationSetsMap);
	}
}

static std::pair<int, int> calculateRegion(int chunkX, int chunkZ)
{
	int regionX = chunkX / REGION_SIZE - int(chunkX < 0);
	int regionZ = chunkZ / REGION_SIZE - int(chunkZ < 0);
	return std::pair<int, int>(regionX, regionZ);
}

static void insertSavedChunkLocationSet(std::pair<int, int> region, int chunkX, int chunkZ)
{
	SavedChunksLocationSetsMap[region].insert(SAVEINDEX(chunkX, chunkZ));
}

std::string calculateFilename(std::pair<int, int> region)
{
	std::string filename = std::string(SAVE_FOLDER);
	std::string regionstr = std::to_string(region.first) + "," + std::to_string(region.second);
	filename += regionstr;
	return filename;
}

static void writeToFile(std::pair<int, int> region, const Chunk& chunk)
{
	std::unordered_map<int, std::string> regionalChunkSet;

	std::string filename = calculateFilename(region);
	if (doesFileExists(filename.c_str()))
	{
		std::ifstream is(filename, std::ios::binary);
		cereal::BinaryInputArchive archive(is);
		archive(regionalChunkSet);
	}

	{
		int index = SAVEINDEX(chunk.getX(), chunk.getZ());
		static char* buffer = new char[16 * 16 * 256];
		int sizecompressed = LZ4_compress_default((char*)chunk.getBlocks(), buffer, 16 * 16 * 256, 16 * 16 * 256);
		regionalChunkSet[index].insert(regionalChunkSet[index].begin(), buffer, buffer + sizecompressed);

		std::ofstream os(filename, std::ios::binary);
		cereal::BinaryOutputArchive archive(os);
		archive(regionalChunkSet);
	}
}

static void readFromFile(std::pair<int, int> region, Chunk& chunk)
{
	std::unordered_map<int, std::string> regionalChunkSet;

	std::string filename = calculateFilename(region);
	{
		std::unordered_map<int, std::string> regionalChunkSet;

		std::ifstream is(filename, std::ios::binary);
		cereal::BinaryInputArchive archive(is);
		archive(regionalChunkSet);

		int index = SAVEINDEX(chunk.getX(), chunk.getZ());
		int sizedecompressed = LZ4_decompress_safe(regionalChunkSet[index].data(), (char*)chunk.getBlocks(), regionalChunkSet[index].size(), 16 * 16 * 256);
	}
}

void Serialize::serializeChunk(const Chunk& chunk)
{
	int chunkX = chunk.getX();
	int chunkZ = chunk.getZ();
	std::pair<int, int> region = calculateRegion(chunkX, chunkZ);
	insertSavedChunkLocationSet(region, chunkX, chunkZ);
	writeToFile(region, chunk);
}

static bool doesSaveFileExist(std::pair<int, int> region, int chunkX, int chunkZ)
{
	if(SavedChunksLocationSetsMap.find(region) != SavedChunksLocationSetsMap.end())
	{
		return SavedChunksLocationSetsMap.at(region).find(SAVEINDEX(chunkX, chunkZ)) != SavedChunksLocationSetsMap.at(region).end();
	}
	return false;
}

bool Serialize::tryDeserializeChunk(Chunk& chunk)
{
	int chunkX = chunk.getX();
	int chunkZ = chunk.getZ();
	std::pair<int, int> region = calculateRegion(chunkX, chunkZ);
	if(doesSaveFileExist(region, chunkX, chunkZ))
	{
		readFromFile(region, chunk);
		return true;
	}
	return false;
}

void Serialize::CloseAndSaveSavedChunksLocationSetsMap()
{
	{
		std::cout << "saved" << std::endl;
		std::ofstream os(REGION_FILE, std::ios::binary);
		cereal::BinaryOutputArchive archive(os);
		archive(SavedChunksLocationSetsMap);
		for (auto a : SavedChunksLocationSetsMap)
		{
			std::cout << "region " << a.first.first << " " << a.first.second << std::endl;
			for (auto b : a.second)
			{
				int x = b % REGION_SIZE + a.first.first * REGION_SIZE;
				int z = b / REGION_SIZE + a.first.second * REGION_SIZE;
				std::cout << x << " " << z << std::endl;
			}
		}
	}
}

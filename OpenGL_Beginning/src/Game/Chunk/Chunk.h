#pragma once
#include <array>
#include <atomic>
#define INDEX(X,Z,Y) (X + CHUNK_WIDTH * Z + Y * CHUNK_WIDTH * CHUNK_LENGTH)

constexpr int CHUNK_WIDTH = 16;    //x
constexpr int CHUNK_LENGTH = 16;   //z
constexpr int CHUNK_HEIGHT = 256;  //y

class Chunk
{
private:
	int x, z, y;
	std::array<uint8_t, CHUNK_WIDTH* CHUNK_LENGTH* CHUNK_HEIGHT> blocks;
	size_t hash;
public:
	std::atomic<bool> treeReady;
	std::array<uint8_t, CHUNK_WIDTH* CHUNK_LENGTH> highest;
	size_t getHash();
	const uint8_t* getBlocks() const { return blocks.data(); };
	bool isMeshReady = false;
	int getX() const { return x; };
	int getZ() const { return z; };
	int getY() const { return y; };
	uint8_t getBlock(uint8_t x, uint8_t z, uint8_t y) const { return blocks[INDEX(x, z, y)]; };
	void setBlock(int INDEX, char blockID) { blocks[INDEX] = blockID; };
	Chunk(int x, int z, int y);
	bool isChunkChanged = false;  // this means it has to be saved, does not mean it has to be loaded from the save file that is decided from scanning save files.
};

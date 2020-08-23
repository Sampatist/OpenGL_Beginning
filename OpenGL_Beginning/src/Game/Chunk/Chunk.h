#pragma once
#include <array>
#define INDEX(X,Z,Y) (X + CHUNK_WIDTH * Z + Y * CHUNK_WIDTH * CHUNK_LENGTH)

constexpr int CHUNK_WIDTH = 16;    //x
constexpr int CHUNK_LENGTH = 16;   //z
constexpr int CHUNK_HEIGHT = 256;  //y

class Chunk
{
private:
	int x, z, y;
	std::array<uint8_t, CHUNK_WIDTH* CHUNK_LENGTH* CHUNK_HEIGHT> blocks;
public:
	bool isMeshReady = false;
	int getX() const { return x; };
	int getZ() const { return z; };
	int getY() const { return y; };
	uint8_t getBlock(uint8_t x, uint8_t z, uint8_t y) const { return blocks[INDEX(x, z, y)]; };
	void setBlock(int INDEX, char blockID) { blocks[INDEX] = blockID; };
	Chunk(int x, int z, int y);
};

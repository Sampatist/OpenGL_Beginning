#pragma once
#include <array>
#define INDEX(X,Z,Y) (X + CHUNK_WIDTH * Z + Y * CHUNK_WIDTH * CHUNK_LENGTH)

constexpr int CHUNK_WIDTH = 16;   //x
constexpr int CHUNK_LENGTH = 16;  //z
constexpr int CHUNK_HEIGHT = 256; //y

class Chunk
{
private:
	int x, z, y;
	std::array<char, CHUNK_WIDTH * CHUNK_LENGTH * CHUNK_HEIGHT> blocks;
public:
	Chunk(int x, int z, int y);
	~Chunk();
};

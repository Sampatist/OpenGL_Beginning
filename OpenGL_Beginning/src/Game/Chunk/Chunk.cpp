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
	hash = (size_t)z ^ ((size_t)x + 0x9e3779b9 + ((size_t)z<<6) + ((size_t)z>>2));
}

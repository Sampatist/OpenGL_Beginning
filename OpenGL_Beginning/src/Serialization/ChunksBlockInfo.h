#pragma once
#include "Chunk/Chunk.h"

constexpr int REGION_SIZE = 32;
#define SAVEINDEX(X,Z) ((X % REGION_SIZE + (X < 0) * REGION_SIZE) + REGION_SIZE * (Z % REGION_SIZE + (Z < 0) * REGION_SIZE))


namespace Serialize
{
	void initialize();
	void serializeChunk(const Chunk& chunk);
	bool tryDeserializeChunk(Chunk& chunk);
	void CloseAndSaveSavedChunksLocationSetsMap();
}

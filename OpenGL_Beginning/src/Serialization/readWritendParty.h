#pragma once
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

namespace Serialize
{
	void writendreadTests();
	//void Chunk();
}

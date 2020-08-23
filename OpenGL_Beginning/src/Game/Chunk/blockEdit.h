#pragma once
#include "Camera.h"
#include "ChunkManager.h"
#include "PhysicsEngine/rayCast.h"

namespace BlockEdit
{
	RayCast::Info getCurrentRayInfo();
	void update();
}
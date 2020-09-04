#pragma once
#include "glm/vec3.hpp"
#include "PhysicsObject.h"

struct CollisionInfo
{
	float time;
	glm::vec3 normal;
	glm::vec3 hitboxPosition;
};

CollisionInfo SweptAABB(const PhysicsObject& b1, const HitBox& b2);
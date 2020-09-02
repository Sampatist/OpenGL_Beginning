#pragma once
#include "glm/vec3.hpp"
#include "PhysicsObject.h"

struct CollisionInfo
{
	float time;
	glm::vec3 normal;
};

CollisionInfo SweptAABB(const PhysicsObject& b1, const HitBox& b2);
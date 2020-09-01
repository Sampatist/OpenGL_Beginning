#pragma once

struct CollisionInfo
{
	float time;
	glm::vec3 normal;
};

CollisionInfo SweptAABB(PhysicsObject& b1, HitBox& b2);
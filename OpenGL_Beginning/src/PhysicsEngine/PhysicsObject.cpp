#include "PhysicsObject.h"
#include "Chunk/ChunkManager.h"
#include "rayCast.h"
#include <array>
#include "glm/trigonometric.hpp"

constexpr float dragCoef = 0.05f;
const glm::vec3 gravity(0.0f, -1.8f, 0.0f);
//TODO

void PhysicsObject::update()
{
	auto drag = 1 - pow(glm::length(velocity), 2) * dragCoef;
	acceleration = (currentForce + gravity) / mass * drag;
	currentForce = glm::vec3(0);

	velocity += acceleration;
	float velocityLength = glm::length(velocity);

	if (velocityLength > 0)
	{
		glm::vec3 unitVelocity(normalize(velocity));

		std::vector<RayCast::Info> infos;

		for(int i = 0; i < 8; i++)
		{
			auto point = position + hitbox.points[i];
			RayCast::Info info = RayCast::castRayAndGetTheInfoPlease(point, unitVelocity, velocityLength, 15);
			if (info.hit)
			{
				infos.push_back(info);
			}
		}
		
		if(!infos.empty())
		{
			const auto minLengthSort = [](RayCast::Info& first, RayCast::Info& second) {
				return first.rayLength < second.rayLength;
			};
			std::sort(infos.begin(), infos.end(), minLengthSort);

			for(int i = 0; i < 8; i++)
			{
				if (infos.size() == i)
					break;
				RayCast::Info& collisionInfo = infos[i];
				glm::vec3 collisionResponseVector = collisionInfo.faceNormal * glm::dot(velocity, -collisionInfo.faceNormal);
				std::cout << "collisionResponseVector: " << collisionResponseVector.x << " " << collisionResponseVector.y << " " << collisionResponseVector.z << std::endl;
				if(abs(collisionInfo.faceNormal.x) == 1)
				{
					velocity.x = 0;
				}
				else if(abs(collisionInfo.faceNormal.z) == 1)
				{
					velocity.z = 0;
				}
				else
				{
					velocity.y = 0;
				}
			}
			// position -= collisionResponseVector;
		}
	}

	position += velocity;

	if (glm::length(acceleration) == 0)
		velocity *= 0.9f;
	if(glm::length(velocity) < 0.001f)
		velocity = glm::vec3(0);
}

void PhysicsObject::addForce(glm::vec3 force)
{
	currentForce += force;
}

glm::vec3 PhysicsObject::getPosition() const
{
	return position;
}

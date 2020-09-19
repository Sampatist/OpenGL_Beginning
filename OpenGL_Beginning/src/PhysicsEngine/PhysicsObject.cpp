#include "PhysicsObject.h"
#include "Chunk/ChunkManager.h"
#include "rayCast.h"
#include <array>
#include "glm/trigonometric.hpp"
#include "BroadPhasedHitBoxes.h"
#include "Renderer.h"
#include "Time.h"

constexpr double dragCoef = 0.15;
const glm::vec<3, double, glm::packed_highp> gravity(0.0, 0.0, 0.0);
constexpr double MAXSPEED = 100.0;

void PhysicsObject::update()
{
	Renderer::drawDebugBox({ glm::vec<3, double, glm::packed_highp>(position.x - this->hitbox.w / 2,position.y - this->hitbox.h / 2,position.z - this->hitbox.d / 2), hitbox.w, hitbox.h, hitbox.d,  glm::vec<3, double, glm::packed_highp>(1.0f, 0.0f, 0.7f) });
	
	auto drag = 1 - pow(glm::length(velocity), 2) * dragCoef;

	acceleration = (((currentForce) / mass) + Time::getDeltaRealGameTime() * 
		(hasGravity ? gravity : glm::vec<3, double, glm::packed_highp>(0.0f))) * double(drag==drag);

	currentForce = glm::vec<3, double, glm::packed_highp>(0);

	velocity.x += acceleration.x * Time::getDeltaGameTime();
	velocity.z += acceleration.z * Time::getDeltaGameTime();
	velocity.y += acceleration.y;

	isOnGround = false;
	int collisionCount = 0;
	double remainingTime = 1.0f;

	std::vector<HitBox> blocks = getBroadPhasedHitBoxes(*this);
	int count = 0;

	while (collisionCount++ < 3 && remainingTime > 0)
	{
		std::vector<CollisionInfo> infos;
		
		for (auto& block : blocks)
		{
			infos.push_back(SweptAABB(*this, block));
		}

		if (!infos.empty())
		{
			const auto leastTimeSort = [&](const CollisionInfo& first, const CollisionInfo& second) {
				if(first.time == second.time)
				{
					float a = glm::length(first.hitboxPosition - position);
					float b = glm::length(second.hitboxPosition - position);
					return a < b;
				}
				return first.time < second.time;
			};

			std::sort(infos.begin(), infos.end(), leastTimeSort);

			CollisionInfo& firstCollisionInfo = infos.front();
			double collisionTime = firstCollisionInfo.time;

			if (remainingTime <= collisionTime)
				break;

			remainingTime = remainingTime - collisionTime;

			position += velocity * collisionTime;

			velocity -= glm::dot(velocity, firstCollisionInfo.normal) * firstCollisionInfo.normal;

			// normal.y is -1 if on ground
			// this is fucking weird man
			if (firstCollisionInfo.normal.y == -1.0f && firstCollisionInfo.hitboxPosition.y < position.y)
			{
				groundTime++;
				isOnGround = true;
			}
		}
	}
	if (!isOnGround)
		groundTime = 0;

	position += velocity * remainingTime;

	if (isOnGround)
	{
		velocity.x *= 0.8f;
		velocity.z *= 0.8f;
	}
	else 
	{

		//velocity.x *= 0.95f;
		//velocity.z *= 0.95f;

	}
	//if (hasGravity && !isOnGround)
	//	velocity.y *= 0.99f;
	if(glm::length(velocity) < 0.0001f)
		velocity = glm::vec3(0);
	//if (glm::length(velocity) > MAXSPEED)
	//	velocity = normalize(velocity) * MAXSPEED;
}

void PhysicsObject::addForce(glm::vec<3, double, glm::packed_highp> force)
{
	currentForce += force;
} 

glm::vec<3, double, glm::packed_highp> PhysicsObject::getPosition() const
{
	return position;
}

glm::vec<3, double, glm::packed_highp> PhysicsObject::getVelocity() const
{
	return velocity;
}

void PhysicsObject::setGravity(bool value)
{
	hasGravity = value;
}

HitBox PhysicsObject::getHitBox() const
{
	HitBox realHitBox = hitbox;
	realHitBox.x = position.x - hitbox.w / 2;
	realHitBox.y = position.y - hitbox.h / 2;
	realHitBox.z = position.z - hitbox.d / 2;
	return realHitBox;
}


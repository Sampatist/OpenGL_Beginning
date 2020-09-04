#include "PhysicsObject.h"
#include "Chunk/ChunkManager.h"
#include "rayCast.h"
#include <array>
#include "glm/trigonometric.hpp"
#include "Swept_AABB.h"
#include "Renderer.h"

constexpr float dragCoef = 0.05f;
const glm::vec3 gravity(0.0f, -0.5f, 0.0f);
//TODO
constexpr float MAXSPEED = 1.0f;

std::vector<HitBox> getBroadPhasedHitBoxes(const PhysicsObject& p)
{
	HitBox b = p.getHitBox();
	glm::vec3 v = p.getVelocity();

	std::vector<HitBox> hitboxes;

	float x = v.x > 0 ? b.x : b.x + v.x;  
	float y = v.y > 0 ? b.y : b.y + v.y;  
	float z = v.z > 0 ? b.z : b.z + v.z;

	float w = v.x > 0 ? v.x + b.w : b.w - v.x;  
	float h = v.y > 0 ? v.y + b.h : b.h - v.y;  
	float d = v.z > 0 ? v.z + b.d : b.d - v.z;  

	for(int k = floor(y); k < ceil(y + h); k++)
	{
		if (abs(k - 128) > 128)
		{
			continue;
		}
		for(int j = floor(z); j < ceil(z + d); j++)
		{
			for(int i = floor(x); i < ceil(x + w); i++)
			{
				int chunkX = floor(float(i) / CHUNK_WIDTH);
				int chunkZ = floor(float(j) / CHUNK_LENGTH);
				int chunkBlockX = i % CHUNK_WIDTH;
				chunkBlockX = chunkBlockX + (chunkBlockX < 0) * CHUNK_WIDTH;
				int chunkBlockZ = j % CHUNK_LENGTH;
				chunkBlockZ = chunkBlockZ + (chunkBlockZ < 0) * CHUNK_LENGTH;
				int chunkBlockY = k;
				std::pair<int, int> chunkLocation(chunkX, chunkZ);
				ChunkManager::loadedChunksLock.lock();
				if(auto chunk = ChunkManager::lock_getChunk(chunkLocation))
				{

					if(auto blockID = chunk->getBlock(chunkBlockX, chunkBlockZ, chunkBlockY))
					{
						hitboxes.push_back({ (float)i,(float)k,(float)j,1.0f,1.0f,1.0f });
					}
				}
				ChunkManager::loadedChunksLock.unlock();
			}
		}
	}
	Renderer::drawDebugBox({ {b.x, b.y, b.z}, b.w, b.h, b.d, {1.0f,0.0f,0.0f} });
	return hitboxes;
}

void PhysicsObject::update()
{
	auto drag = 1 - pow(glm::length(velocity), 2) * dragCoef;
	acceleration = (currentForce + gravity * float(!Creative)) / mass * drag;
	currentForce = glm::vec3(0);

	velocity += acceleration;

	isOnGround = 0;
	int collisionCount = 0;
	float remainingTime = 1.0f;

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

			//const auto uniqueNormal = [&](const CollisionInfo& first, const CollisionInfo& second) {
			//	return first.normal == second.normal;
			//};

			std::sort(infos.begin(), infos.end(), leastTimeSort);

			//infos.erase(std::remove_if(infos.begin(), infos.end(), removeNotHit), infos.end());

			//infos.erase(std::unique(infos.begin(), infos.end(), uniqueNormal), infos.end());

			CollisionInfo& firstCollisionInfo = infos.front();
			float collisionTime = firstCollisionInfo.time;

			if (remainingTime <= collisionTime)
				break;

			remainingTime = remainingTime - collisionTime;
			//std::cout << "position before collision: 1_pos(" << position.x << ", " << position.y << ", " << position.z << ") \n";
			//std::cout << "old velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ") \n";
			//position += velocity * collisionTime;
			//std::cout << "CollisionCount: " << collisionCount << std::endl;
			//std::cout << "position after collision: 2_pos(" << position.x << ", " << position.y << ", " << position.z << ") \n";

			//velocity -= glm::dot(velocity, firstCollisionInfo.normal) * firstCollisionInfo.normal;

			float r = 0;
			float g = 0;
			float b = 0;
			{
				auto& info = infos.front();
				if(count == 0)
				{
					r = 1;
				}else if(count == 1)
				{
					r = 0;
					g = 1;
				}else if(count == 2)
				{
					g = 0;
					b = 1;
				}
				auto length = glm::length(info.hitboxPosition - position);
				Renderer::drawDebugBox({ {info.hitboxPosition.x - 0.5,info.hitboxPosition.y- 0.5,info.hitboxPosition.z- 0.5}, 1 + count / 10.0f, 1, 1, {r,g,b} });
				//std::cout << info.time << r << " " << g << " " << b << std::endl;
				count++;
			}

			//std::cout << std::endl;

			if(abs(firstCollisionInfo.normal.x) > 0)
			{
				velocity.x = 0;
			}
			else if(abs(firstCollisionInfo.normal.y) > 0)
			{
				//std::cout << "CollisionCount: " << collisionCount << std::endl << "y equal to 0" << std::endl;
				velocity.y = 0;
			}
			else if(abs(firstCollisionInfo.normal.z) > 0)
			{
				velocity.z = 0;
			}

			//std::cout << "new velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ") \n";
			if (firstCollisionInfo.normal.y == -1.0f && firstCollisionInfo.hitboxPosition.y < position.y)
			{
				isOnGround = 1;
			}
		}
	}

	position += velocity * remainingTime;
	/*glm::vec3 unitVelocity = glm::length(velocity) > 0 ? normalize(velocity) : glm::vec3(0.0f);
	glm::vec3 unitAcceleration = glm::length(acceleration) > 0 ? normalize(acceleration) : glm::vec3(0.0f);

	velocity.x *= std::max<float>(unitAcceleration.x * unitVelocity.x, 0.99f);
	velocity.z *= std::max<float>(unitAcceleration.z * unitVelocity.z, 0.99f);*/
	if(glm::length(velocity) < 0.001f)
		velocity = glm::vec3(0);
	if (glm::length(velocity) > MAXSPEED)
		velocity = normalize(velocity) * MAXSPEED;
}

void PhysicsObject::addForce(glm::vec3 force)
{
	currentForce += force;
} 

glm::vec3 PhysicsObject::getPosition() const
{
	return position;
}

glm::vec3 PhysicsObject::getVelocity() const
{
	return velocity;
}

HitBox PhysicsObject::getHitBox() const
{
	HitBox realHitBox = hitbox;
	realHitBox.x = position.x - hitbox.w / 2;
	realHitBox.y = position.y - hitbox.h / 2;
	realHitBox.z = position.z - hitbox.d / 2;
	return realHitBox;
}


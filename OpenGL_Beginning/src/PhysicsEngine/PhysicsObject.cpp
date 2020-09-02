#include "PhysicsObject.h"
#include "Chunk/ChunkManager.h"
#include "rayCast.h"
#include <array>
#include "glm/trigonometric.hpp"
#include "Swept_AABB.h"
#include "Renderer.h"

constexpr float dragCoef = 0.05f;
const glm::vec3 gravity(0.0f, -1.0f, 0.0f);
//TODO

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
						Renderer::drawDebugBox({ {i,k,j}, 1, 1, 1, {0.0f,0.0f,1.0f} });
						hitboxes.push_back({ (float)i,(float)k,(float)j,1.0f,1.0f,1.0f });
					}
				}
				ChunkManager::loadedChunksLock.unlock();
			}
		}
	}
	Renderer::drawDebugBox({ {b.x, b.y, b.z}, b.w, b.h, b.d, {1.0f,0.0f,0.0f} });
	std::cout << b.x << " " << b.z << std::endl;
	return hitboxes;
}

void PhysicsObject::update()
{
	auto drag = 1 - pow(glm::length(velocity), 2) * dragCoef;
	acceleration = (currentForce) / mass * drag;
	currentForce = glm::vec3(0);

	velocity += acceleration;
	float velocityLength = glm::length(velocity);

	std::vector<HitBox> blocks = getBroadPhasedHitBoxes(*this);
	std::vector<CollisionInfo> infos;

	for(auto& block : blocks)
	{
		infos.push_back(SweptAABB(*this, block));
	}

	// remove duplicate normals from infos vector

	if (!infos.empty())
	{
		const auto leastTimeSort = [](const CollisionInfo& first, const CollisionInfo& second){
			return first.time < second.time;
		};

		std::sort(infos.begin(), infos.end(), leastTimeSort);

		CollisionInfo& firstCollisionInfo = infos.front();
		float collisionTime = firstCollisionInfo.time;
		float remainingTime = 1 - collisionTime;
	
		position += velocity * collisionTime;

 		velocity += - glm::dot(velocity, firstCollisionInfo.normal) * firstCollisionInfo.normal;

		position += velocity * remainingTime;
	}
	else 
	{
		position += velocity;
	}

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


#pragma once
#include "glm/vec3.hpp"
#include <array>

struct HitBox
{
	// position of negx - negy - negz corner 
	float x, y, z;

	// Dimensions
	float w, h, d;
};

class PhysicsObject
{	
private:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float mass;
	glm::vec3 currentForce;
	HitBox hitbox;
	bool hasGravity;
public:
	bool isOnGround;
	float groundTime;
public:
	PhysicsObject(glm::vec3 startingPosition, float mass, HitBox hitbox)
		: position(startingPosition), velocity(0), acceleration(0), 
		mass(mass), currentForce(0), hitbox(hitbox), hasGravity(true), isOnGround(0), groundTime(0) {}

	void update();
	void addForce(glm::vec3 force);
	void setGravity(bool value);
	glm::vec3 getPosition() const;
	glm::vec3 getVelocity() const;
	HitBox getHitBox() const;
};
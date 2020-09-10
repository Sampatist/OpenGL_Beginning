#pragma once
#include "glm/vec3.hpp"
#include <array>

struct HitBox
{
	// position of negx - negy - negz corner 
	double x, y, z;

	// Dimensions
	double w, h, d;
};

class PhysicsObject
{	
private:
	glm::vec<3, double, glm::packed_highp> position;
	glm::vec<3, double, glm::packed_highp> velocity;
	glm::vec<3, double, glm::packed_highp> acceleration;
	double mass;
	glm::vec<3, double, glm::packed_highp> currentForce;
	HitBox hitbox;
	bool hasGravity;
public:
	bool isOnGround;
	float groundTime;
public:
	PhysicsObject(glm::vec<3, double, glm::packed_highp> startingPosition, double mass, HitBox hitbox)
		: position(startingPosition), velocity(0), acceleration(0), 
		mass(mass), currentForce(0), hitbox(hitbox), hasGravity(true), isOnGround(0), groundTime(0) {}

	void update();
	void addForce(glm::vec<3, double, glm::packed_highp> force);
	void setGravity(bool value);
	glm::vec<3, double, glm::packed_highp> getPosition() const;
	glm::vec<3, double, glm::packed_highp> getVelocity() const;
	HitBox getHitBox() const;
};
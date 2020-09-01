#pragma once
#include "glm/vec3.hpp"
#include <array>

struct HitBox
{
	std::array<glm::vec3, 8> points;

	HitBox(float width, float height, float depth)
	{
		points[0] = glm::vec3(  width / 2,  height / 2,  length / 2 );  // posXposYposZ
		points[1] = glm::vec3(  width / 2,  height / 2, -length / 2 );	// posXposYnegZ
		points[2] = glm::vec3( -width / 2,  height / 2,  length / 2 );	// negXposYposZ
		points[3] = glm::vec3( -width / 2,  height / 2, -length / 2 );	// negXposYnegZ
							 										 
		points[4] = glm::vec3(  width / 2, -height / 2,  length / 2 );	// posXnegYposZ
		points[5] = glm::vec3(  width / 2, -height / 2, -length / 2 );	// posXnegYnegZ
		points[6] = glm::vec3( -width / 2, -height / 2,  length / 2 );	// negXnegYposZ
		points[7] = glm::vec3( -width / 2, -height / 2, -length / 2 );	// negXnegYnegZ
	}	
};

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
public:
	PhysicsObject(glm::vec3 startingPosition, float mass, HitBox hitbox)
		: position(startingPosition), velocity(0), acceleration(0), 
		mass(mass), currentForce(0), hitbox(hitbox) {}

	void update();
	void addForce(glm::vec3 force);
	glm::vec3 getPosition() const;
	glm::vec3 getVelocity() const;
	HitBox getHitBox() const;
};
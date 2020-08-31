#pragma once
#include "glm/vec3.hpp"
#include <array>

struct HitBox
{
	std::array<glm::vec3, 8> points;

	////top
	//glm::vec3 posXposYposZ;
	//glm::vec3 posXposYnegZ;
	//glm::vec3 negXposYposZ;
	//glm::vec3 negXposYnegZ;
	////bottom
	//glm::vec3 posXnegYposZ;
	//glm::vec3 posXnegYnegZ;
	//glm::vec3 negXnegYposZ;
	//glm::vec3 negXnegYnegZ;
	 
	HitBox(float width, float length, float height)
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
};
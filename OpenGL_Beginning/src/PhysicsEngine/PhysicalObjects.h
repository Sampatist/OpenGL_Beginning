#pragma once
#include "glm/vec3.hpp"

struct Movement
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

};
namespace Player
{
	//void calcColision(glm::vec3 pos, glm::vec3 dir);
	void calcAcceleration();
	void calcVelocity();
	void CalculateNewPosition();

	void updatePos();

	void ChangeCameraPosAndAngle();
}
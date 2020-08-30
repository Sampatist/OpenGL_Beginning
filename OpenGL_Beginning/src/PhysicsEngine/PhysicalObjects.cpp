#include "PhysicalObjects.h"
#include "Camera.h"
#include "inputs.h"
#include "Chunk/ChunkManager.h"
#include "rayCast.h"

constexpr float PI = 3.14f;

constexpr float SENSITIVITY = 0.0005f;

constexpr float SPEED = 0.6f; //0.2f feels good  this is actually the accelertion speed or smthng like that
constexpr float SPEED_MULTIPLIER = 5.5f;  //dont know
constexpr float MAX_VELOCITY = 10.0f; 

constexpr float YAW_ANGLE_UPPER_LIMIT = PI / 2 - 0.1f;
constexpr float YAW_ANGLE_LOWER_LIMIT = -PI / 2 + 0.05f;

static	glm::vec3 up_vector(0.0f, 1.0f, 0.0f);
static	glm::vec3 right_vector(0.0f);
static	glm::vec3 forward_vector(0.0f);


static glm::vec3 acceleration = glm::vec3(0.0f);
static glm::vec3 velocity = glm::vec3(0.0f);

//NOT A GOOD IDEA 
//void Player::calcColision(glm::vec3 pos, glm::vec3 dir)
//{
//	collisionInfo.clear();
//
//	bool head_posXcolisionInfo = RayCast::castRayAndGetTheInfoPlease(pos, glm::vec3(  1.0f, 0.0f,  0.0f), 0.5f, 1).hit;
//	bool head_negXcolisionInfo = RayCast::castRayAndGetTheInfoPlease(pos, glm::vec3( -1.0f, 0.0f,  0.0f), 0.5f, 1).hit;
//	bool head_posZcolisionInfo = RayCast::castRayAndGetTheInfoPlease(pos, glm::vec3(  0.0f, 0.0f,  1.0f), 0.5f, 1).hit;
//	bool head_negZcolisionInfo = RayCast::castRayAndGetTheInfoPlease(pos, glm::vec3(  0.0f, 0.0f, -1.0f), 0.5f, 1).hit;
//
//	bool body_posXcolisionInfo = RayCast::castRayAndGetTheInfoPlease(pos - glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(  1.0f, 0.0f,  0.0f), 0.5f, 1).hit;
//	bool body_negXcolisionInfo = RayCast::castRayAndGetTheInfoPlease(pos - glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3( -1.0f, 0.0f,  0.0f), 0.5f, 1).hit;
//	bool body_posZcolisionInfo = RayCast::castRayAndGetTheInfoPlease(pos - glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(  0.0f, 0.0f,  1.0f), 0.5f, 1).hit;
//	bool body_negZcolisionInfo = RayCast::castRayAndGetTheInfoPlease(pos - glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(  0.0f, 0.0f, -1.0f), 0.5f, 1).hit;
//
//	bool head_posYcolisionInfo = RayCast::castRayAndGetTheInfoPlease(pos, glm::vec3(0.0f,  1.0f, 0.0f), 0.5f, 1).hit;
//	bool body_negYcolisionInfo = RayCast::castRayAndGetTheInfoPlease(pos, glm::vec3(0.0f, -1.0f, 0.0f), 0.5f, 1).hit;
//
//	bool posXCollision = head_posXcolisionInfo && body_posXcolisionInfo;
//
//	collisionInfo.push_back(posXCollision);
//
//	collisionInfo.push_back(head_negXcolisionInfo);
//	collisionInfo.push_back(head_posZcolisionInfo);
//	collisionInfo.push_back(head_negZcolisionInfo);
//
//	collisionInfo.push_back(body_negXcolisionInfo);
//	collisionInfo.push_back(body_posZcolisionInfo);
//	collisionInfo.push_back(body_negZcolisionInfo);
//												
//	collisionInfo.push_back(head_posYcolisionInfo);
//	collisionInfo.push_back(body_negYcolisionInfo);
//}


void Player::calcAcceleration()
{
	float right = inputManager::getInput(inputManager::Input::RIGHT);
	float forward = inputManager::getInput(inputManager::Input::FORWARD);
	float jump = inputManager::getInput(inputManager::Input::FLY);
	float shift = inputManager::getInput(inputManager::Input::SHIFT);

	right_vector = glm::normalize(glm::cross(Camera::GetCameraAngle(), up_vector));
	forward_vector = glm::normalize(glm::cross(up_vector, right_vector));

	glm::vec3 accelerationInfo = right_vector * right + forward_vector * forward + up_vector * jump;
	glm::vec3 unitAcceleration = glm::abs(glm::length(accelerationInfo)) ? glm::normalize(accelerationInfo) : glm::vec3(0.0f);

	acceleration = unitAcceleration * SPEED * (shift == 1.0f ? SPEED_MULTIPLIER : 1.0f) - up_vector * 0.3f;
}

void Player::calcVelocity()
{
	velocity = velocity + acceleration;
	float velocityLength = glm::length(velocity);
	if (velocityLength >= MAX_VELOCITY)
		velocity = MAX_VELOCITY * normalize(velocity);

	glm::vec3 velocity_x = glm::vec3(velocity.x, 0.0f, 0.0f);
	float velocity_xLength = glm::length(velocity_x);

	if (velocity_xLength > 0)
	{
		glm::vec3 unitVelocity_x(normalize(velocity_x));
		RayCast::Info collisionInfoHead = RayCast::castRayAndGetTheInfoPlease(Camera::GetPosition(), unitVelocity_x, velocity_xLength, 10);
		if (collisionInfoHead.hit)
		{
			if (glm::length(velocity_x) < 0.5f)
				velocity_x = glm::vec3(0.0f);
			float newVelocityLength = collisionInfoHead.rayLenght - 0.5f;
			std::cout << newVelocityLength << std::endl;
			velocity_x = unitVelocity_x * (newVelocityLength);
		}
	}

	glm::vec3 velocity_z = glm::vec3(0.0f, 0.0f, velocity.z);
	float velocity_zLength = glm::length(velocity_z);

	if (velocity_zLength > 0)
	{
		glm::vec3 unitVelocity_z(normalize(velocity_z));
		RayCast::Info collisionInfoHead = RayCast::castRayAndGetTheInfoPlease(Camera::GetPosition(), unitVelocity_z, velocity_zLength, 10);
		if (collisionInfoHead.hit)
		{
			if (glm::length(velocity_z) < 0.5f)
				velocity_z = glm::vec3(0.0f);
			float newVelocityLength = collisionInfoHead.rayLenght - 0.5f;
			std::cout << newVelocityLength << std::endl;
			velocity_z = unitVelocity_z * (newVelocityLength);
		}
	}
	
	glm::vec3 velocity_y = glm::vec3(0.0f, velocity.y, 0.0f);
	float velocity_yLength = glm::length(velocity_y);

	if (velocity_yLength > 0)
	{
		glm::vec3 unitVelocity_y(normalize(velocity_y));
		RayCast::Info collisionInfoHead = RayCast::castRayAndGetTheInfoPlease(Camera::GetPosition(), unitVelocity_y, velocity_yLength, 10);
		if (collisionInfoHead.hit)
		{
			if (glm::length(velocity_y) < 0.5f)
				velocity_y = glm::vec3(0.0f);
			std::cout << collisionInfoHead.rayLenght << std::endl;
			float newVelocityLength = collisionInfoHead.rayLenght - 0.5f;
			std::cout << newVelocityLength << std::endl;
			velocity_y = unitVelocity_y * (newVelocityLength);
		}
	}

	velocity = velocity_x + velocity_z + velocity_y;
	velocity = velocity * 0.9f;
	if (glm::length(velocity) < 0.01f)
		velocity = glm::vec3(0.0f);

	//if (velocityLength > 0)
	//{
	//	glm::vec3 unitVelocity(normalize(velocity));
	//	RayCast::Info collisionInfoHead = RayCast::castRayAndGetTheInfoPlease(Camera::GetPosition(), normalize(unitVelocity), velocityLength, 10);
	//	RayCast::Info collisionInfoBody = RayCast::castRayAndGetTheInfoPlease(Camera::GetPosition() - up_vector, normalize(unitVelocity), velocityLength, 10);
	//	RayCast::Info collisionInfoLegs = RayCast::castRayAndGetTheInfoPlease(Camera::GetPosition() - 2.0f * up_vector, normalize(unitVelocity), velocityLength, 10);
	//
	//	if (collisionInfoHead.hit)
	//	{
	//		float newVelocityLength = collisionInfoHead.rayLenght - 0.5f;
	//		std::cout << newVelocityLength << std::endl;
	//		velocity = unitVelocity * (newVelocityLength);
	//	}
	//	if (collisionInfoBody.hit)
	//	{
	//		float newVelocityLength = collisionInfoBody.rayLenght - 0.5f;
	//		std::cout << newVelocityLength << std::endl;
	//		velocity = unitVelocity * (newVelocityLength);
	//	}
	//	if (collisionInfoLegs.hit)
	//	{
	//		float newVelocityLength = collisionInfoLegs.rayLenght - 0.5f;
	//		std::cout << newVelocityLength << std::endl;
	//		velocity = unitVelocity * (newVelocityLength);
	//	}
	//}



}

void Player::CalculateNewPosition()
{
	Camera::setPosition(Camera::GetPosition() + velocity);
}

void Player::updatePos()
{
	calcAcceleration();
	calcVelocity();
	CalculateNewPosition();
	std::cout << "acceleration: " << acceleration.x << " " << acceleration.z << " " << acceleration.y << " " << std::endl;

	std::cout << "velocity: " << velocity.x << " " << velocity.z << " " << velocity.y << " " << std::endl;
}

void Player::ChangeCameraPosAndAngle()
{
}

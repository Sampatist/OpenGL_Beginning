#include "PlayerController.h"
#include "Camera.h"
#include "inputs.h"
#include <iostream>
#include <algorithm>
#include "PhysicsEngine/blockEdit.h"

constexpr float PI = 3.14f;

constexpr float SENSITIVITY = 0.0005f;

constexpr float SPEED = 0.2f;
constexpr float SPEED_MULTIPLIER = 2.5f;

constexpr float YAW_ANGLE_UPPER_LIMIT = PI/2 - 0.1f;
constexpr float YAW_ANGLE_LOWER_LIMIT = -PI/2 + 0.05f;

static	glm::vec3 up_vector(0.0f, 1.0f, 0.0f);
static	glm::vec3 right_vector(0.0f);
static	glm::vec3 forward_vector(0.0f);

static float mouseX = 0.0f;
static float mouseY = 0.0f;

void PlayerController::update()
{
	//CAMERA
	float deltaMouseX = inputManager::getInput(inputManager::Input::MOUSEX);
	float deltaMouseY = inputManager::getInput(inputManager::Input::MOUSEY);

	//MOVEMENT
	float right = inputManager::getInput(inputManager::Input::RIGHT);
	float forward = inputManager::getInput(inputManager::Input::FORWARD);
	float fly = inputManager::getInput(inputManager::Input::FLY);
	float shift = inputManager::getInput(inputManager::Input::SHIFT);

	//MOUSE ACTION
	float mouseLeft = inputManager::getInput(inputManager::Input::MOUSELEFT);
	float mouseRight = inputManager::getInput(inputManager::Input::MOUSERIGHT);

	if(mouseLeft)
	{
		BlockEdit::RayCastInfo info = BlockEdit::getCurrentRayInfo();
		//std::cout << "does left click work?" << std::endl;
		//std::cout << "info: " << info.hit << std::endl << (int)info.direction << std::endl
		//	<< info.block.blockID << std::endl << info.block.x << std::endl 
		//	<< info.block.z << std::endl << info.block.y << std::endl;
		if(info.hit)
		{
			//std::cout << "does info.hit work?" << std::endl;

			ChunkManager::loadedChunksLock.lock();
			ChunkManager::loadedChunksMap[info.block.location]->setBlock(INDEX(info.block.x, info.block.z, info.block.y), 0);
			ChunkManager::loadedChunksLock.unlock();
			Renderer::blockUpdate = true;
		}
	}
	if(mouseRight == 1)
	{
		BlockEdit::RayCastInfo info = BlockEdit::getCurrentRayInfo();

		if (info.hit)
		{
			//std::cout << "does info.hit work?" << std::endl;
			ChunkManager::loadedChunksLock.lock();
			ChunkManager::loadedChunksMap[info.block.location]->setBlock(INDEX(info.block.x, info.block.z, info.block.y) + (int)info.direction , info.block.blockID);
			ChunkManager::loadedChunksLock.unlock();
			Renderer::blockUpdate = true;
		}
	}

	mouseX += deltaMouseX * SENSITIVITY;
	mouseY += deltaMouseY * SENSITIVITY;
	mouseY = std::clamp(mouseY, YAW_ANGLE_LOWER_LIMIT, YAW_ANGLE_UPPER_LIMIT);

	Camera::setAngle(mouseY, mouseX);

	right_vector = glm::normalize(glm::cross(Camera::GetCameraAngle(), up_vector));
	forward_vector = glm::normalize(glm::cross(up_vector, right_vector));

	glm::vec3 translate = right_vector * right + forward_vector * forward + up_vector * fly;

	Camera::setPosition(
		Camera::GetPosition() + 
		(glm::abs(glm::length(translate)) ? 
		 glm::normalize(translate) : 
		 glm::vec3(0.0f)) * 
		 SPEED * (shift == 1.0f ? SPEED_MULTIPLIER : 1.0f));
}
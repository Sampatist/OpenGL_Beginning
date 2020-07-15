#include "CameraController.h"
#include "inputs.h"
#include <iostream>
#include <algorithm>

constexpr float PI = 3.14f;

constexpr float SENSITIVITY = 0.0005f;

constexpr float SPEED = 0.01f;
constexpr float SPEED_MULTIPLIER = 2.5f;

constexpr float YAW_ANGLE_UPPER_LIMIT = PI/2;
constexpr float YAW_ANGLE_LOWER_LIMIT = -PI/2;

void CameraController::update()
{
	//CAMERA
	float cumulativeMousex = -inputManager::getInput(inputManager::Input::MOUSEX);
	float cumulativeMousey = -inputManager::getInput(inputManager::Input::MOUSEY);

	//MOVEMENT
	float right = inputManager::getInput(inputManager::Input::RIGHT);
	float forward = inputManager::getInput(inputManager::Input::FORWARD);
	float fly = inputManager::getInput(inputManager::Input::FLY);
	float shift = inputManager::getInput(inputManager::Input::SHIFT);

	float mouseyWithLimit = std::max(YAW_ANGLE_LOWER_LIMIT, std::min(cumulativeMousey * SENSITIVITY, YAW_ANGLE_UPPER_LIMIT));

	camera.setAngle(mouseyWithLimit, cumulativeMousex * SENSITIVITY);

	right_vector = glm::normalize(glm::cross(camera.GetCameraAngle(), up_vector));
	forward_vector = glm::normalize(glm::cross(up_vector, right_vector));

	glm::vec3 translate = right_vector * right + forward_vector * forward + up_vector * fly;

	camera.setPosition(
		camera.GetPosition() + 
		(glm::abs(glm::length(translate)) ? 
		 glm::normalize(translate) : 
		 glm::vec3(0.0f)) * 
		 SPEED * (shift == 1.0f ? SPEED_MULTIPLIER : 1.0f));
}
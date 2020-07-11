#pragma once
#include "Camera.h"
#include "inputs.h"
#include "glm/vec3.hpp"

class CameraController
{
private:
	Camera camera;
	glm::vec3 up_vector;
	glm::vec3 right_vector;
	glm::vec3 forward_vector;
public:
	CameraController()
		: camera(Camera(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f))),
		up_vector(0.0f, 1.0f, 0.0f) ,
		right_vector(0.0f),
		forward_vector(0.0f)
	{
	};
	
	void update();

	const Camera& getCamera() const { return camera; };
};
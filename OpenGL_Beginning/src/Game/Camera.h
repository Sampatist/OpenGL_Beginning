#pragma once
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

class Camera {

private:
	glm::vec3 Position;
	glm::vec3 CameraAngle;
	glm::vec3 InitialAngle;

public:
	Camera(glm::vec3 StartingPosition, glm::vec3 StartingAngle)
		: Position(StartingPosition), CameraAngle(StartingAngle), InitialAngle(StartingAngle) 
	{};

	void setAngle(float pitch, float yaw);
	void setPosition(glm::vec3 value);

	glm::vec3 GetPosition() const { return Position; };
	glm::vec3 GetCameraAngle() const { return CameraAngle; };
};
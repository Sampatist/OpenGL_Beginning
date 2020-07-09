#pragma once
#include <glm/vec3.hpp>

class Camera {

private:
	glm::vec3 Position;
	glm::vec3 CameraAngle;
public:
	Camera(glm::vec3 StartingPosition, glm::vec3 StartingAngle);
	void UpdatePosition(glm::vec3 PositionUpdate);
	void UpdateAngle(glm::vec3 AngleUpdate);

	glm::vec3 GetPosition() const { return Position; };
	glm::vec3 GetCameraAngle() const { return CameraAngle; };
};
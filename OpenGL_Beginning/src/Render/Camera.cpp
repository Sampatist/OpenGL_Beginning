#include "Camera.h"

Camera::Camera(glm::vec3 StartingPosition, glm::vec3 StartingAngle)
{ 
	Position = StartingPosition;
	CameraAngle = StartingAngle;
}

void Camera::UpdatePosition(glm::vec3 PositionUpdate)
{
	Position += PositionUpdate;
}

void Camera::UpdateAngle(glm::vec3 AngleUpdate)
{
	CameraAngle += AngleUpdate;
}

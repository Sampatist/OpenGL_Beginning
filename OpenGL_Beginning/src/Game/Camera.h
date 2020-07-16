#pragma once
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

namespace Camera {
	void setAngle(float pitch, float yaw);
	void setPosition(glm::vec3 value);

	const glm::vec3& GetPosition();
	const glm::vec3& GetCameraAngle();
};
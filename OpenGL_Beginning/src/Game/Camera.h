#pragma once
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

namespace Camera {
	void setAngle(float pitch, float yaw);
	void setPosition(glm::vec<3, double, glm::packed_highp> value);
	const glm::vec<3, double, glm::packed_highp>& GetPosition();
	const glm::vec<3, double, glm::packed_highp>& GetCameraAngle();
	const int GetRelativeCamXOffsetCoeff();
	const int GetRelativeCamZOffsetCoeff();
	const glm::dvec3 GetRelativeCamPosition();
};
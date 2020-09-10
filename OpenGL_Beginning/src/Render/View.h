#pragma once

#include <glm/mat4x4.hpp>
#include <Camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Settings.h"

namespace ViewFrustum
{
	void initialize();
	void update();
	glm::mat<4, 4, double, glm::packed_highp> getViewMatrix();
	glm::mat<4, 4, double, glm::packed_highp> getProjMatrix();
	bool contains2D(glm::vec2 pos);
}

//inline glm::mat4 ViewMatrix() {
//
//	return glm::lookAt(Camera::GetPosition(), Camera::GetPosition() + Camera::GetCameraAngle(), glm::vec3(0.0f, 1.0f, 0.0f));
//}

//inline glm::mat4 ProjectionMatrix(float AspectRatio) {
//
//	//return glm::ortho(0.0f, 30.0f, 0.0f, 30.0f, Settings::ZNEAR, Settings::ZFAR);
//	return glm::perspective(glm::radians(Settings::fov), AspectRatio, Settings::ZNEAR, Settings::ZFAR);
//}
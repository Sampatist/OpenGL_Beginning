#pragma once

#include <glm/mat4x4.hpp>
#include <Camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Settings.h"

constexpr float ZNEAR = 0.1f;
constexpr float ZFAR = 1320.0f;

inline glm::mat4 ViewMatrix() {

	return glm::lookAt(Camera::GetPosition(), Camera::GetPosition() + Camera::GetCameraAngle(), glm::vec3(0.0f, 1.0f, 0.0f));
}

inline glm::mat4 ProjectionMatrix(float AspectRatio) {

	return glm::perspective(glm::radians(Settings::fov), AspectRatio, ZNEAR, ZFAR);
}
#pragma once

#include <glm/mat4x4.hpp>
#include <Camera.h>
#include <glm/gtc/matrix_transform.hpp>
constexpr float ZNEAR = 0.1f;
constexpr float ZFAR = 320.0f;


inline glm::mat4 ViewMatrix(const Camera& Camera) {

	return glm::lookAt(Camera.GetPosition(), Camera.GetPosition() + Camera.GetCameraAngle(), glm::vec3(0.0f, 1.0f, 0.0f));
}

inline glm::mat4 ProjectionMatrix(float fov, float AspectRatio) {

	return glm::perspective(glm::radians(fov), AspectRatio, ZNEAR, ZFAR);
}

										//CameraMatrix 	x	ViewMatrix	x	ModelMatrix   x	  VertexPosition

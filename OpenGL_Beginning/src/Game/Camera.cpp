#include "Camera.h"
#include "glm/trigonometric.hpp"

void Camera::setAngle(float pitchAngle, float yawAngle)
{
    glm::mat3 pitchRotationMatrix(
        1, 0, 0,
        0, glm::cos(pitchAngle), -glm::sin(pitchAngle),
        0, glm::sin(pitchAngle), glm::cos(pitchAngle)
    );
    glm::mat3 yawRotationMatrix(
        glm::cos(yawAngle), 0, glm::sin(yawAngle),
        0, 1, 0,
        -glm::sin(yawAngle), 0, glm::cos(yawAngle)
    );
    CameraAngle = yawRotationMatrix * pitchRotationMatrix  * InitialAngle;
}

void Camera::setPosition(glm::vec3 value)
{
    Position = value;
}


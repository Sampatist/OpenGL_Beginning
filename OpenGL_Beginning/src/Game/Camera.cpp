#include "Camera.h"
#include "glm/trigonometric.hpp"
#include <glm/detail/type_vec3.hpp>

static	glm::vec<3, double, glm::packed_highp> Position(30.0, 125.0, 2.0);
static	glm::vec<3, double, glm::packed_highp> CameraAngle(0.0, 0.0, -1.0);
static	glm::vec3 InitialAngle(0.0f, 0.0f, -1.0f);

void Camera::setAngle(float pitchAngle, float yawAngle)
{
    const glm::mat3 pitchRotationMatrix(
        1, 0, 0,
        0, glm::cos(pitchAngle), -glm::sin(pitchAngle),
        0, glm::sin(pitchAngle), glm::cos(pitchAngle)
    );
    const glm::mat3 yawRotationMatrix(
        glm::cos(yawAngle), 0, glm::sin(yawAngle),
        0, 1, 0,
        -glm::sin(yawAngle), 0, glm::cos(yawAngle)
    );
    CameraAngle = yawRotationMatrix * pitchRotationMatrix  * InitialAngle;
}

void Camera::setPosition(glm::vec<3, double, glm::packed_highp> value)
{
    Position = value;
}

const glm::vec<3, double, glm::packed_highp>& Camera::GetPosition()
{
    return Position;
}

const glm::vec<3, double, glm::packed_highp>& Camera::GetCameraAngle()
{
    return CameraAngle;
}

#include "Camera.h"
#include "glm/trigonometric.hpp"
#include <glm/detail/type_vec3.hpp>

static glm::vec<3, double, glm::packed_highp> Position(1000000.0, 225.0, 0.0);
static glm::vec<3, double, glm::packed_highp> RelativePosition(Position);
static glm::vec<3, double, glm::packed_highp> CameraAngle(0.0, 0.0, -1.0);
static glm::vec<3, double, glm::packed_highp> InitialAngle(CameraAngle);

constexpr int HOWRELATIVE = 10000;

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
    RelativePosition = Position - glm::dvec3(1.0, 0.0, 0.0) * (double)GetRelativeCamXOffsetCoeff() - glm::dvec3(0.0, 0.0, 1.0) * (double)GetRelativeCamZOffsetCoeff();
}

const glm::vec<3, double, glm::packed_highp>& Camera::GetPosition()
{
    return Position;
}

const glm::vec<3, double, glm::packed_highp>& Camera::GetCameraAngle()
{
    return CameraAngle;
}

const int Camera::GetRelativeCamXOffsetCoeff() 
{
    return ((int)Camera::GetPosition().x / HOWRELATIVE) * HOWRELATIVE;
}

const int Camera::GetRelativeCamZOffsetCoeff()
{
    return ((int)Camera::GetPosition().z / HOWRELATIVE) * HOWRELATIVE;
}

const glm::dvec3 Camera::GetRelativeCamPosition()
{
    return RelativePosition;
}
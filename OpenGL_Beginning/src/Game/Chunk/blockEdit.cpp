#include "blockEdit.h"

static RayCast::Info lastViewRayInfo;

constexpr float RAY_LENGTH = 10;
constexpr int RAY_LIMIT = 50;

RayCast::Info BlockEdit::getCurrentRayInfo()
{
	return lastViewRayInfo;
}

void BlockEdit::update()
{
	lastViewRayInfo = RayCast::castRayAndGetTheInfoPlease(Camera::GetPosition(), Camera::GetCameraAngle(), RAY_LENGTH, RAY_LIMIT);
}


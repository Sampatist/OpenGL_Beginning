#include "blockEdit.h"

static RayCast::Info lastViewRayInfo;

constexpr float RAY_LENGTH = 200;
constexpr int RAY_LIMIT = 3000;

RayCast::Info BlockEdit::getCurrentRayInfo()
{
	return lastViewRayInfo;
}

void BlockEdit::update()
{
	lastViewRayInfo = RayCast::castRayAndGetTheInfoPlease(Camera::GetPosition(), Camera::GetCameraAngle(), RAY_LENGTH, RAY_LIMIT);
}


#pragma once

namespace inputManager
{
	enum class Input
	{
		RIGHT,
		FORWARD,
		MOUSEX,
		MOUSEY,
		JUMP,
		FLY,
		CTRL,
		SHIFT,
		MOUSELEFT,
		MOUSERIGHT,
	};
	float getInput(Input input);
	void initialize();
	void update();
}

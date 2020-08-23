#include "inputs.h"
#include "input_handlers/KeyboardEventHandler.h"
#include "input_handlers/MouseEventHandler.h"
#include "Settings.h"
#include "Renderer.h"

static float right = 0.0f;
static float forward = 0.0f;
static double deltaMouseX = 0.0f;
static double deltaMouseY = 0.0f;
static float fly = 0.0f;
static float shift = 0.0f;
static float mouseRight = 0.0f;
static float mouseLeft = 0.0f;

float inputManager::getInput(Input input)
{
	switch(input)
	{
	case Input::FORWARD:
		return forward;
	case Input::RIGHT:
		return right;
	case Input::MOUSEX:
		return deltaMouseX;
	case Input::MOUSEY:
		return deltaMouseY;
	case Input::FLY:
		return fly;
	case Input::SHIFT:
		return shift;
	case Input::MOUSERIGHT:
		return mouseRight;
	case Input::MOUSELEFT:
		return mouseLeft;
	}
}

void inputManager::initialize()
{
	input::bindKeyboard(Renderer::getWindow());
	input::bindMouse(Renderer::getWindow());
	input::addMouseBinding(&mouseRight, Settings::place);
	input::addMouseBinding(&mouseLeft, Settings::dig);
	input::addKeyboardBinding(&forward, Settings::forward, Settings::back);
	input::addKeyboardBinding(&right, Settings::right, Settings::left);
	input::addKeyboardBinding(&fly, Settings::up, Settings::down);
	input::addKeyboardBinding(&shift, GLFW_KEY_LEFT_SHIFT);
}

void inputManager::update()
{
	input::updateMouse(&deltaMouseX, &deltaMouseY);
}
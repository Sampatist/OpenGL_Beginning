#include "inputs.h"
#include "input_handlers/KeyboardEventHandler.h"
#include "input_handlers/MouseEventHandler.h"
#include "Settings.h"
#include "Renderer.h"

static float right = 0.0f;
static float forward = 0.0f;
static float mousex = 0.0f;
static float mousey = 0.0f;
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
		return mousex;
	case Input::MOUSEY:
		return mousey;
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
	input::updateMouse(&mousex, &mousey);
	input::addMouseBinding(&mouseRight, Settings::place);
	input::addMouseBinding(&mouseLeft, Settings::dig);
	input::addKeyboardBinding(&forward, Settings::forward, Settings::back);
	input::addKeyboardBinding(&right, Settings::right, Settings::left);
	input::addKeyboardBinding(&fly, Settings::up, Settings::down);
	input::addKeyboardBinding(&shift, GLFW_KEY_LEFT_SHIFT);
}
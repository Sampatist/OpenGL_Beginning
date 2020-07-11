#include "inputs.h"
#include "input_handlers/KeyboardEventHandler.h"
#include "input_handlers/MouseEventHandler.h"

static float right = 0.0f;
static float forward = 0.0f;
static float mousex = 0.0f;
static float mousey = 0.0f;
static float fly = 0.0f;
static float shift = 0.0f;

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
	}
}

void inputManager::initialize(GLFWwindow* window)
{
	input::bindKeyboard(window);
	input::bindMouse(window);
	input::updateMouse(&mousex, &mousey);
	input::addKeyboardBinding(&forward, GLFW_KEY_W, GLFW_KEY_S);
	input::addKeyboardBinding(&right, GLFW_KEY_D, GLFW_KEY_A);
	input::addKeyboardBinding(&fly, GLFW_KEY_SPACE, GLFW_KEY_LEFT_CONTROL);
	input::addKeyboardBinding(&shift, GLFW_KEY_LEFT_SHIFT);

}
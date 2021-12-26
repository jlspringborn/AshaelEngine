#include "Input.h"

namespace ash
{
	Input::Input()
	{
	}

	Input::~Input()
	{
	}

	void Input::update()
	{
		glfwPollEvents();
	}
}
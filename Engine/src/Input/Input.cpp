/**
 * Copyright (C) 2021, Jesse Springborn
 */
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
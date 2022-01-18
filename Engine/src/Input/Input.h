/**
 * Handles GLFW Input events
 *
 * Copyright (C) 2022, Jesse Springborn
 */
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace ash
{
	class Input
	{
	public:
		Input();
		~Input();

		/**
		 * Poll input events using glfw
		 */
		void update();

	private:

	};
}
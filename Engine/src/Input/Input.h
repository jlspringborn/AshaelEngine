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
		 * @brief poll input events using glfw
		 */
		void update();

	private:

	};
}
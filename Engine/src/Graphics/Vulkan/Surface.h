#pragma once

#include "Vulkan/Instance.h"
#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace ash
{
	class Surface
	{
	public:
		Surface(const Instance* instance, const Window* window);
		~Surface();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkSurfaceKHR& () const { return m_surface; }

	private:
		VkSurfaceKHR m_surface{};

		const Instance* m_instance{};
	};
}
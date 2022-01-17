/**
* Wrapper for Vulkan Surface
*
* Copyright (C) 2022, Jesse Springborn
*/
#pragma once

#include "Vulkan/Instance.h"
#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace ash
{
	/**
	 * Wrapper for Vulkan Surface
	 */
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
		
		/**
		 * Vulkan Surface, surface to write image data to for presentation to the screen
		 */
		VkSurfaceKHR m_surface{};

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const Instance* m_instance{};
	};
}
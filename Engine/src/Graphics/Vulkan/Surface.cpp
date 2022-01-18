/**
 * Copyright (C) 2021, Jesse Springborn
 */
#include "Vulkan\Surface.h"

#include <stdexcept>

namespace ash
{
	Surface::Surface(const Instance* instance, const Window* window) :
		m_instance{ instance }
	{
		if (glfwCreateWindowSurface(*m_instance, window->getWindow(), nullptr, &m_surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
	}

	Surface::~Surface()
	{
		vkDestroySurfaceKHR(*m_instance, m_surface, nullptr);
	}
}
/**
 * Manages system window
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace ash
{
	class Window
	{
	public:
		Window(const int width, const int height);
		~Window();

		/**
		 * @brief checks if close button was pressed
		 */
		bool shouldClose() const { return glfwWindowShouldClose(m_window); }

		GLFWwindow* getWindow() const { return m_window; }

		const VkExtent2D getWindowExtent() const;

		const bool getWasWindowResized() const { return wasWindowResized; }

		void resetWasWindowResized();

	private:

		// glfw window
		GLFWwindow* m_window{};	

		bool wasWindowResized{ false };

		static void framebufferResizedCallback(GLFWwindow* window, int width, int height)
		{
			auto appWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
			appWindow->wasWindowResized = true;
		}

	};
}
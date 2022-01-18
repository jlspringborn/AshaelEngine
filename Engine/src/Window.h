/**
 * Wrapper for GLFW Window
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>	// for logging

namespace ash
{
	/**
	 * Wrapper for GLFW Window
	 */
	class Window
	{
	public:
		Window(const int width, const int height);
		~Window();

		/**
		 * Checks if close button was pressed
		 */
		bool shouldClose() const { return glfwWindowShouldClose(m_window); }

		/**
		 * Sets application to close during next loop iteration
		 */
		void closeWindow() { glfwSetWindowShouldClose(m_window, GLFW_TRUE); }

		/**
		 * Returns the GLFWwindow pointer
		 */
		GLFWwindow* getWindow() const { return m_window; }

		/**
		 * Returns current window dimensions
		 */
		const VkExtent2D getWindowExtent() const;

		/**
		 * Used for window resized callback
		 */
		const bool getWasWindowResized() const { return wasWindowResized; }

		/**
		 * Used for window resized callback
		 */
		void resetWasWindowResized();

		/**
		 * Mouse coordinates
		 */
		float lastX{};
		float lastY{};
		float yaw{};
		float pitch{};

		/**
		 * Prevents jumping during initial mouse movement
		 */
		bool firstMouse{ true };

	private:

		/**
		 * GLFW window
		 */
		GLFWwindow* m_window{};	

		/**
		 * Used for window resized callback
		 */
		bool wasWindowResized{ false };

		/**
		 * Called from GLFW when the window gets resized
		 */
		static void framebufferResizedCallback(GLFWwindow* window, int width, int height)
		{
			auto appWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
			appWindow->wasWindowResized = true;
		}

		/**
		 * Called from GLFW when mouse input is detected
		 */
		static void mouseCallback(GLFWwindow* window, double xpos, double ypos)
		{
			auto appWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

			if (appWindow->firstMouse)
			{
				appWindow->lastX = static_cast<float>(xpos);
				appWindow->lastY = static_cast<float>(ypos);
				appWindow->firstMouse = false;
			}

			float xoffset = static_cast<float>(xpos) - appWindow->lastX;
			float yoffset = appWindow->lastY - static_cast<float>(ypos);
			appWindow->lastX = static_cast<float>(xpos);
			appWindow->lastY = static_cast<float>(ypos);

			float sensitivity = 0.1f;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			appWindow->yaw += xoffset;
			appWindow->pitch += yoffset;

			if (appWindow->pitch > 89.0f)
				appWindow->pitch = 89.0f;
			if (appWindow->pitch < -89.0f)
				appWindow->pitch = -89.0f;
		}

	};
}
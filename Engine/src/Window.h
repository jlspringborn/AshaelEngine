/**
 * Manages system window
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>	// for logging

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

		float lastX{};

		float lastY{};

		float xoffset{};

		float yoffset{};

		float yaw{};

		float pitch{};

		bool firstMouse{ true };

	private:

		// glfw window
		GLFWwindow* m_window{};	

		bool wasWindowResized{ false };

		static void framebufferResizedCallback(GLFWwindow* window, int width, int height)
		{
			auto appWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
			appWindow->wasWindowResized = true;
		}

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
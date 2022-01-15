#include "Window.h"


namespace ash
{
	Window::Window(const int width, const int height) :
		lastX{ static_cast<float>(width) / 2.f }, lastY{ static_cast<float>(height) / 2.f }
	{
		glfwInit();	// starts glfw

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	// needed if not using opengl

		m_window = glfwCreateWindow(width, height, "Ashael", nullptr, nullptr);	// create the window
		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, framebufferResizedCallback);

		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(m_window, mouseCallback);
	}

	Window::~Window()
	{
		// cleanup 
		glfwDestroyWindow(m_window);	
		glfwTerminate();
	}

	const VkExtent2D Window::getWindowExtent() const
	{
		int width;
		int height;

		glfwGetFramebufferSize(m_window, &width, &height);

		VkExtent2D extent =
		{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		return extent;
	}

	void Window::resetWasWindowResized()
	{
		wasWindowResized = false;
	}

	//void Window::framebufferResizedCallback(GLFWwindow* window, int width, int height)
	
}
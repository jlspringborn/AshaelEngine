#include "App.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>


namespace ash
{
	App::App() :
		m_window(std::make_unique<Window>(1920, 1080)),
		m_input(std::make_unique<Input>()),
		m_graphics(std::make_unique<Graphics>(m_window.get()))
	{
	}

	App::~App()
	{
	}

	void App::run()
	{
		while (!m_window->shouldClose())
		{
			m_input->update();
			m_graphics->renderGameObjects();
		}
		
		m_graphics->waitForDeviceIdle();
	}
}
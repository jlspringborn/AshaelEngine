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
		loadGameObjects();

		while (!m_window->shouldClose())
		{
			m_input->update();
			m_graphics->renderGameObjects(m_gameObjects);
		}
		
		m_graphics->waitForDeviceIdle();

		cleanupGameObjects();
	}

	void App::loadGameObjects()
	{
		std::unique_ptr<Model> model = std::make_unique<Model>(m_graphics->m_logicalDevice.get(), m_graphics->m_physicalDevice.get(),
			m_graphics->m_swapChain->getImageCount(), m_graphics->m_descriptorSetLayout, m_graphics->m_descriptorPool->getPool(), m_graphics->m_textureSampler, m_graphics->m_uniformBuffers);
		m_gameObjects.push_back(std::move(model));

		std::unique_ptr<Model> model2 = std::make_unique<Model>(m_graphics->m_logicalDevice.get(), m_graphics->m_physicalDevice.get(),
			m_graphics->m_swapChain->getImageCount(), m_graphics->m_descriptorSetLayout, m_graphics->m_descriptorPool->getPool(), m_graphics->m_textureSampler, m_graphics->m_uniformBuffers);
		model2->setOffset({0.0f, -0.5f, -0.5f});
		m_gameObjects.push_back(std::move(model2));
	}

	void App::cleanupGameObjects()
	{
		for (size_t i = 0; i < m_gameObjects.size(); i++)
		{
			m_gameObjects[i] = nullptr;
		}
	}
}
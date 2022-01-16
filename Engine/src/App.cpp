#include "App.h"

#include "GameObjects/GameObject.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>


namespace ash
{
	App::App() :
		m_window(std::make_unique<Window>(1920, 1080)),
		m_input(std::make_unique<Input>()),
		m_graphics(std::make_unique<Graphics>(m_window.get()))
	{
		m_camera = new Camera();
		m_cameraController = new CameraController();
	}

	App::~App()
	{
	}

	void App::run()
	{
		loadGameObjects();

		auto currentTime = std::chrono::high_resolution_clock::now();

		auto viewerObject = new GameObject();
		viewerObject->m_transformComponent.setTranslation(glm::vec3{ 0.0f, -.5f, -2.5f });

		while (!m_window->shouldClose())
		{
			m_input->update();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			//frameTime = glm::min(frameTime, m_maxFrameTime)
			currentTime = newTime;

			m_camera->update(m_graphics->getAspectRatio());
			m_cameraController->moveInPlaneXZ(m_window.get(), frameTime, viewerObject);
			m_camera->setViewDirection(viewerObject->m_transformComponent.m_translation, m_cameraController->m_forwardDirection);
			//m_camera->setViewYXZ(viewerObject->m_transformComponent.m_translation, viewerObject->m_transformComponent.m_rotation);

			m_graphics->renderGameObjects(m_gameObjects, m_camera);
		}
		
		m_graphics->waitForDeviceIdle();
	}

	void App::loadGameObjects()
	{
		std::unique_ptr<GameObject> gameObject = 
			std::make_unique<GameObject>(m_graphics->generateModel("models/viking_room_adjusted.obj", "textures/viking_room.png"));
		//gameObject->m_transformComponent.setTranslation(glm::vec3{ 0.0f, 0.0f, 20.f });
		m_gameObjects.push_back(std::move(gameObject));

		std::unique_ptr<GameObject> gameObject2 =
			std::make_unique<GameObject>(m_graphics->generateModel("models/viking_room_adjusted.obj", "textures/viking_room.png"));
		gameObject2->m_transformComponent.setTranslation(glm::vec3{ -1.5f, 0.0f, 0.f });
		m_gameObjects.push_back(std::move(gameObject2));

	}
}
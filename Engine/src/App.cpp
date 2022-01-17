#include "App.h"

#include "GameObjects/GameObject.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>


namespace ash
{
	// TODO: remove hard coded window size
	App::App() :
		m_window(std::make_unique<Window>(1920, 1080)),
		m_input(std::make_unique<Input>()),
		m_graphics(std::make_unique<Graphics>(m_window.get())),
		m_camera(std::make_unique<Camera>()),
		m_cameraController(std::make_unique<CameraController>())
	{

	}

	App::~App()
	{
	}

	void App::run()
	{
		// TODO: load from scene file
		loadGameObjects();

		auto currentTime = std::chrono::high_resolution_clock::now();

		// Create the viewer object which will contain the camera's position and rotation
		auto viewerObject = new GameObject();
		viewerObject->m_transformComponent.setTranslation(glm::vec3{ 0.0f, -.5f, -2.5f });

		while (!m_window->shouldClose())
		{
			m_input->update();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			// TODO: add max frame time variable to prevent jitter during extended resizing
			//frameTime = glm::min(frameTime, m_maxFrameTime) 
			currentTime = newTime;

			// TODO: move camera function calls to it's own function to cleanup loop
			// Set camera projection mode, this updates because the aspect ratio can change during a swap chain recreation
			m_camera->setPerspectiveProjection(glm::radians(45.0f), m_graphics->getAspectRatio(), 0.1f, 10.f);

			// Move camera controller and update forward direction based on mouse input
			m_cameraController->moveInPlaneXZ(m_window.get(), frameTime, viewerObject);

			// Set view direction
			m_camera->setViewDirection(viewerObject->m_transformComponent.m_translation, m_cameraController->m_forwardDirection);

			// Render provided game objects based on provided camera view and projection matrix info
			m_graphics->renderGameObjects(m_gameObjects, m_camera.get());
		}
		
		// Prevents error if scene closes before Vulkan process is finished
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
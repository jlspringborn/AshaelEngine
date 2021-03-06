/**
 * Initializes components and manages core loop
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Window.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "GameObjects/GameObject.h"
#include "Camera/Camera.h"
#include "Camera/CameraController.h"

#include <memory>	// for unique_ptr
#include <vector>	// for GameObject array

namespace ash
{
	class App
	{
	public:
		App();
		~App();
		
		/**
		 * Called by game script to start engine
		 */
		void run();

		/**
		 * TODO: load objects from scene file instead of manually adding them
		 */
		void loadGameObjects();

	private:

		/**
		 * GLFW Window, used to create platform agnostic window for presenting
		 */
		std::unique_ptr<Window> m_window{};

		/**
		 * TODO: process input and emit signals
		 */
		std::unique_ptr<Input> m_input{};

		/**
		 * Manages visual processing and presentation of game scenes
		 */
		std::unique_ptr<Graphics> m_graphics{};

		/**
		 * Keep below graphics in the load order to ensure smart pointer are
		 * freed before Graphics, due to dependency on logical device pointer
		 */
		std::vector<std::unique_ptr<GameObject>> m_gameObjects{};

		/**
		 * Contains view and projection matrices used for rendering
		 */
		std::unique_ptr<Camera> m_camera{};

		/**
		 * Processes input and and adjusts viewerObject transform information
		 * viewerObject is created in the run() function
		 */
		std::unique_ptr<CameraController> m_cameraController{};
	};
}
/**
 * Moves provided viewerObject based on keyboard and mouse input
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "GameObjects/GameObject.h"
#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>	

namespace ash
{
	/**
	 * Moves provided viewerObject based on keyboard and mouse input
	 */
	class CameraController
	{
	public:
		CameraController();
		~CameraController();

		/**
		 * Struct containing GLFW key mapping info
		 */
		struct KeyMappings
		{
			int moveLeft		= GLFW_KEY_A;
			int moveRight		= GLFW_KEY_D;
			int moveForward		= GLFW_KEY_W;
			int moveBackward	= GLFW_KEY_S;
			int moveUp			= GLFW_KEY_E;
			int moveDown		= GLFW_KEY_Q;
			int lookLeft		= GLFW_KEY_LEFT;
			int lookRight		= GLFW_KEY_RIGHT;
			int lookUp			= GLFW_KEY_UP;
			int lookDown		= GLFW_KEY_DOWN;
		};

		/**
		 * Moves the camera based on mouse and keyboard input
		 */
		void move(Window* window, float deltaTime, GameObject* gameObject);

		/**
		 * Returns forward direction
		 */
		const glm::vec3& getForwardDirection() const { return m_forwardDirection; }

	private:

		/**
		 * Struct containing key mapping info
		 */
		KeyMappings m_keys{};

		/**
		 * How fast the camera can move forward/backward, up/down, and side to side
		 */
		float m_moveSpeed{ 3.f };

		/**
		 * How fast the camera can rotate around
		 */
		float m_lookSpeed{ 1.5f };

		/**
		 * The current direction the camera is facing, based on mouse input
		 */
		glm::vec3 m_forwardDirection{0.f, 0.f, -1.f};

		/**
		 * Updates the forward direction based on mouse input
		 */
		void updateForwardDirection(Window* window);

	};
}
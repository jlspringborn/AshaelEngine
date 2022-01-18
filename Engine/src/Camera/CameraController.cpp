/**
 * Copyright (C) 2021, Jesse Springborn
 */
#include "Camera/CameraController.h"

#include <limits>

#include <iostream>

namespace ash
{
	CameraController::CameraController()
	{
	}

	CameraController::~CameraController()
	{
	}

	void CameraController::move(Window* window, float deltaTime, GameObject* gameObject)
	{
		// Update forward direction based on movement
		updateForwardDirection(window);

		// Prevents vertical movement with forward, backward, and side to size keys
		const glm::vec3 forwardDirection{ m_forwardDirection.x, 0.f, m_forwardDirection.z };
		const glm::vec3 rightDirection{ m_forwardDirection.z, 0.f, -m_forwardDirection.x };

		// Set up direction for Vulkan
		const glm::vec3 upDirection{ 0.f, -1.f, 0.f };

		// Key movement key input
		glm::vec3 moveDirection{ 0.f };
		if (glfwGetKey(window->getWindow(), m_keys.moveForward) == GLFW_PRESS) { moveDirection += forwardDirection; }
		if (glfwGetKey(window->getWindow(), m_keys.moveBackward) == GLFW_PRESS) { moveDirection -= forwardDirection; }
		if (glfwGetKey(window->getWindow(), m_keys.moveRight) == GLFW_PRESS) { moveDirection += rightDirection; }
		if (glfwGetKey(window->getWindow(), m_keys.moveLeft) == GLFW_PRESS) { moveDirection -= rightDirection; }
		if (glfwGetKey(window->getWindow(), m_keys.moveUp) == GLFW_PRESS) { moveDirection += upDirection; }
		if (glfwGetKey(window->getWindow(), m_keys.moveDown) == GLFW_PRESS) { moveDirection -= upDirection; }

		// Move if any movement keys were pressed
		if (glm::dot(moveDirection, moveDirection) > std::numeric_limits<float>::epsilon())
		{
			gameObject->getTransform().setTranslation(
				gameObject->getTransform().getTranslation() + m_lookSpeed * deltaTime * glm::normalize(moveDirection));
		}
	}

	void CameraController::updateForwardDirection(Window* window)
	{
		// Set forward direction based on mouse input
		glm::vec3 direction{};
		direction.x = cos(glm::radians(window->yaw)) * cos(glm::radians(window->pitch));
		direction.y = sin(glm::radians(window->pitch));
		direction.z = sin(glm::radians(window->yaw)) * cos(glm::radians(window->pitch));
		m_forwardDirection = glm::normalize(direction);
		m_forwardDirection.x *= -1;
		m_forwardDirection.y *= -1;
	}
}
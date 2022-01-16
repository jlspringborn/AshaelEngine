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

	void CameraController::moveInPlaneXZ(Window* window, float deltaTime, GameObject* gameObject)
	{
		// Set forward direction based on mouse input
		glm::vec3 direction{};
		direction.x = cos(glm::radians(window->yaw)) * cos(glm::radians(window->pitch));
		direction.y = sin(glm::radians(window->pitch));
		direction.z = sin(glm::radians(window->yaw)) * cos(glm::radians(window->pitch));
		m_forwardDirection = glm::normalize(direction);
		m_forwardDirection.x *= -1;
		m_forwardDirection.y *= -1;

		// Rotate based on arrow keys
		//glm::vec3 rotate{ 0 };
		//if (glfwGetKey(window->getWindow(), m_keys.lookRight) == GLFW_PRESS) { rotate.y += 1.f; }
		//if (glfwGetKey(window->getWindow(), m_keys.lookLeft) == GLFW_PRESS) { rotate.y -= 1.f; }
		//if (glfwGetKey(window->getWindow(), m_keys.lookUp) == GLFW_PRESS) { rotate.x += 1.f; }
		//if (glfwGetKey(window->getWindow(), m_keys.lookDown) == GLFW_PRESS) { rotate.x -= 1.f; }

		//if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		//{
		//	gameObject->m_transformComponent.m_rotation += m_lookSpeed * deltaTime * glm::normalize(rotate);
		//}

		//gameObject->m_transformComponent.m_rotation.x = glm::clamp(gameObject->m_transformComponent.m_rotation.x, -1.5f, 1.5f);
		//gameObject->m_transformComponent.m_rotation.y = glm::mod(gameObject->m_transformComponent.m_rotation.y, glm::two_pi<float>());


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
			gameObject->m_transformComponent.m_translation += m_lookSpeed * deltaTime * glm::normalize(moveDirection);
		}
	}

	void CameraController::getMouseMovement(Window* window)
	{
	}
}
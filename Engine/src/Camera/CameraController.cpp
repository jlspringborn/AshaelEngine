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

		glm::vec3 rotate{ 0 };
		if (glfwGetKey(window->getWindow(), m_keys.lookRight) == GLFW_PRESS) { rotate.y += 1.f; }
		if (glfwGetKey(window->getWindow(), m_keys.lookLeft) == GLFW_PRESS) { rotate.y -= 1.f; }
		if (glfwGetKey(window->getWindow(), m_keys.lookUp) == GLFW_PRESS) { rotate.x += 1.f; }
		if (glfwGetKey(window->getWindow(), m_keys.lookDown) == GLFW_PRESS) { rotate.x -= 1.f; }

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		{
			gameObject->m_transformComponent.m_rotation += m_lookSpeed * deltaTime * glm::normalize(rotate);
		}

		gameObject->m_transformComponent.m_rotation.x = glm::clamp(gameObject->m_transformComponent.m_rotation.x, -1.5f, 1.5f);
		gameObject->m_transformComponent.m_rotation.y = glm::mod(gameObject->m_transformComponent.m_rotation.y, glm::two_pi<float>());

		float yaw = gameObject->m_transformComponent.m_rotation.y;
		const glm::vec3 forwardDirection{ sin(yaw), 0.f, cos(yaw) };

		const glm::vec3 rightDirection{ forwardDirection.z, 0.f, -forwardDirection.x };
		const glm::vec3 upDirection{ 0.f, -1.f, 0.f };

		glm::vec3 moveDirection{ 0.f };
		if (glfwGetKey(window->getWindow(), m_keys.moveForward) == GLFW_PRESS) { moveDirection += forwardDirection; }
		if (glfwGetKey(window->getWindow(), m_keys.moveBackward) == GLFW_PRESS) { moveDirection -= forwardDirection; }
		if (glfwGetKey(window->getWindow(), m_keys.moveRight) == GLFW_PRESS) { moveDirection += rightDirection; }
		if (glfwGetKey(window->getWindow(), m_keys.moveLeft) == GLFW_PRESS) { moveDirection -= rightDirection; }
		if (glfwGetKey(window->getWindow(), m_keys.moveUp) == GLFW_PRESS) { moveDirection += upDirection; }
		if (glfwGetKey(window->getWindow(), m_keys.moveDown) == GLFW_PRESS) { moveDirection -= upDirection; }

		if (glm::dot(moveDirection, moveDirection) > std::numeric_limits<float>::epsilon())
		{
			gameObject->m_transformComponent.m_translation += m_lookSpeed * deltaTime * glm::normalize(moveDirection);
		}
	}

	void CameraController::getMouseMovement(Window* window)
	{
	}
}
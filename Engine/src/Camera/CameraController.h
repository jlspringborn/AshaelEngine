#pragma once

#include "GameObjects/GameObject.h"
#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace ash
{
	class CameraController
	{
	public:
		CameraController();
		~CameraController();

		struct KeyMappings
		{
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_E;
			int moveDown = GLFW_KEY_Q;
			int lookLeft = GLFW_KEY_LEFT;
			int lookRight = GLFW_KEY_RIGHT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;
		};

		KeyMappings m_keys{};

		void moveInPlaneXZ(Window* window, float deltaTime, GameObject* gameObject);

		void getMouseMovement(Window* window);

		float m_moveSpeed{ 3.f };

		float m_lookSpeed{ 1.5f };

		glm::vec3 m_forwardDirection{0.f, 0.f, -1.f};

		float m_yaw{};

		float m_pitch{};
	private:

		void updateForwardDirection(Window* window);

	};
}
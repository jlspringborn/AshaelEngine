/**
 * Contains transform information for game objects
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <stdexcept>

namespace ash
{
	class TransformComponent
	{
	public:

		void setTranslation(glm::vec3 translation)
		{
			m_translation = translation;
		}

		void setRotation(glm::vec3 rotation)
		{
			m_rotation = rotation;
		}

		glm::vec3 m_translation{};
		glm::vec3 m_scale{ 1.f };
		glm::vec3 m_rotation{};

		glm::mat4 mat4()
		{
			auto transform = glm::translate(glm::mat4{ 1.f }, m_translation);			// Adjust transform by objects translation
			transform = glm::eulerAngleYXZ(m_rotation.y, m_rotation.x, m_rotation.z);	// Adjust transform by objects rotation
			transform = glm::scale(transform, m_scale);									// Adjust transform by objects scale
			return transform;
		}
	};
}
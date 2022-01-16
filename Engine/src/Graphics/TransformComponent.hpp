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
			const float c3 = glm::cos(m_rotation.z);
			const float s3 = glm::sin(m_rotation.z);
			const float c2 = glm::cos(m_rotation.x);
			const float s2 = glm::sin(m_rotation.x);
			const float c1 = glm::cos(m_rotation.y);
			const float s1 = glm::sin(m_rotation.y);
			return glm::mat4{
				{
					m_scale.x * (c1 * c3 + s1 * s2 * s3),
					m_scale.x * (c2 * s3),
					m_scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					m_scale.y * (c3 * s1 * s2 - c1 * s3),
					m_scale.y * (c2 * c3),
					m_scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					m_scale.z * (c2 * s1),
					m_scale.z * (-s2),
					m_scale.z * (c1 * c2),
					0.0f,
				},
				{m_translation.x, m_translation.y, m_translation.z, 1.0f} };
			//auto transform = glm::translate(glm::mat4{ 1.f }, m_translation);			// Adjust transform by objects translation
			//transform = glm::eulerAngleYXZ(m_rotation.y, m_rotation.x, m_rotation.z);	// Adjust transform by objects rotation
			//transform = glm::scale(transform, m_scale);									// Adjust transform by objects scale
			//return transform;
		}
	};
}
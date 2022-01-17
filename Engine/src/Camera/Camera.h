/**
 * Manages view and projection matrices for rendering
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

namespace ash
{
	/**
	 * Manages view and projection matrices for rendering
	 */
	class Camera
	{
	public:
		Camera();
		~Camera();

		/**
		 * Set projection matrix to orthographic projection mode
		 */
		void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);

		/**
		 * Set projection matrix to perspective projection mode
		 */
		void setPerspectiveProjection(float fovy, float aspect, float near, float far);

		/**
		 * Set view matrix based on position, direction, and up vector
		 * up vector is defaulted to -y for Vulkan
		 */
		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });

		/**
		 * Set view matrix based on positions, target position, and up vector
		 * up vector is defaulted to -y for Vulkan
		 */
		void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });

		/**
		 * Set view matrix based on position and rotation
		 */
		void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

		/**
		 * Returns reference to projection matrix
		 */
		const glm::mat4& getProjection() const { return m_projectionMatrix; }

		/**
		 * Returns reference to view matrix
		 */
		const glm::mat4& getView() const { return m_viewMatrix; }

	private:

		/**
		 * Projection Matrix, modifies field of view, aspect, near plane, and far plane
		 */
		glm::mat4 m_projectionMatrix{ 1.f };

		/**
		 * View Matrix, position data
		 */
		glm::mat4 m_viewMatrix{ 1.f };
	};
}
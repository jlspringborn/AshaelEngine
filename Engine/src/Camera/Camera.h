/**
 * 
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
	class Camera
	{
	public:
		Camera();
		~Camera();

		void update(float aspect);

		void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);

		void setPerspectiveProjection(float fovy, float aspect, float near, float far);

		/**
		 * defaults up direction to -y axis
		 */
		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });

		/**
		 * defaults up direction to -y axis
		 */
		void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });

		void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

		const glm::mat4& getProjection() const { return m_projectionMatrix; }

		const glm::mat4& getView() const { return m_viewMatrix; }

	private:

		glm::mat4 m_projectionMatrix{ 1.f };

		glm::mat4 m_viewMatrix{ 1.f };
	};
}
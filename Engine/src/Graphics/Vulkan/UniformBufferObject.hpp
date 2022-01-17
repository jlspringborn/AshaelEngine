/**
* Contains transform data to be passed to shaders via uniform buffers
*
* Copyright (C) 2022, Jesse Springborn
*/
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ash
{
	/**
	 * Contains transform data to be passed to shaders via uniform buffers
	 */
	struct UniformBufferObject
	{
		glm::mat4 model{1.f};	// TODO: remove this, model matrix is being passed via push constants
		glm::mat4 view{1.f};	// Camera view matrix
		glm::mat4 proj{1.f};	// camera projection matrix
	};
}
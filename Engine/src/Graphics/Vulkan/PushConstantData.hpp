/**
* Struct containing info to be passed to shaders via push constants
*
* Copyright (C) 2021, Jesse Springborn
*/
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace ash
{
	/**
	 * Struct containing info to be passed to shaders via push constants
	 */
	struct PushConstantData
	{
		/**
		 * Transform matrix, used to calculate 3D position
		 */
		glm::mat4 transform;
	};
}
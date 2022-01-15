#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ash
{
	struct UniformBufferObject
	{
		glm::mat4 model{1.f};
		glm::mat4 view{1.f};
		glm::mat4 proj{1.f};
	};
}
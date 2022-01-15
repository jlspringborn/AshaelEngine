#pragma once

#include "Model.h"
#include "TransformComponent.hpp"

#include <memory>

namespace ash
{
	class GameObject
	{
	public:
		GameObject();
		GameObject(std::unique_ptr<Model> model);
		~GameObject();

		void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index);

		TransformComponent m_transformComponent{};

		std::unique_ptr<Model> m_model{};

	private:


	};
}
/**
 * An object that can exist in 3D space
 *
 * Copyright (C) 2021, Jesse Springborn
 */
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

		/**
		 * Pass through function to Model draw command
		 */
		void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index);

		/**
		 * Returns reference to TransformComponent
		 * Must non-const
		 */
		TransformComponent& getTransform() { return m_transformComponent; }

		/**
		 * Returns pointer to model
		 * Must be non-const
		 */
		Model* getModel() { return m_model.get(); }

	private:

		/**
		 * Manages transform data for the GameObject
		 */
		TransformComponent m_transformComponent{};

		/**
		 * Contains vertex and texture data of 3D model
		 */
		std::unique_ptr<Model> m_model{};

	};
}
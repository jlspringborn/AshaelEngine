/**
 * Copyright (C) 2021, Jesse Springborn
 */
#include "GameObjects/GameObject.h"

#include <stdexcept>

namespace ash
{
	GameObject::GameObject()
	{
	}
	GameObject::GameObject(std::unique_ptr<Model> model) :
		m_model{ std::move(model) }
	{
	}

	GameObject::~GameObject()
	{
	}

	void GameObject::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index)
	{
		m_model->draw(commandBuffer, pipelineLayout, index, &m_transformComponent);
	}
}
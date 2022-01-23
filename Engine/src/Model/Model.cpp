/**
 * Copyright (C) 2021, Jesse Springborn
 */

#include "Model.h"

#include "Loaders/ModelLoader.hpp"
#include "Vulkan/UniformBufferObject.hpp"
#include "Vulkan/PushConstantData.hpp"

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>


#include <stdexcept>

namespace ash
{
	Model::Model(
		const LogicalDevice* logicalDevice, 
		const PhysicalDevice* physicalDevice, 
		const int swapChainImageCount, 
		VkDescriptorSetLayout setLayout, 
		VkDescriptorPool pool,
		VkSampler sampler, 
		std::vector<std::unique_ptr<Buffer>>& uniformBuffers,
		std::string modelPath,
		std::string texturePath) :
		m_logicalDevice{ logicalDevice }
	{
		//createTexture(physicalDevice, texturePath);
		loadglTFFile(modelPath, *this, logicalDevice, physicalDevice);
		std::cout << "Vertices count: " << m_vertices.size() << '\n';
		//loadModel(modelPath, m_vertices, m_indices);
		createVertexBuffer(physicalDevice);
		createIndexBuffer(physicalDevice);
		//createUniformBuffers(physicalDevice, swapChainImageCount);

		std::cout << "Image count: " << m_textureImages.size() << '\n';
		std::cout << "Texture count: " << m_textures.size() << '\n';
		std::cout << "Material count: " << m_materials.size() << '\n';
	}

	Model::~Model()
	{
	}

	void Model::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index,TransformComponent* transform)
	{
		VkBuffer vertexBuffers[] = { *m_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		
		//vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		//vkCmdBindIndexBuffer(commandBuffer, *m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		PushConstantData push{};
		auto newTransform = transform->mat4();
		push.transform = transform->mat4();

		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);
		/*
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);*/

		// All vertices and indices are stored in single buffers, so we only need to bind once

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, *m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		// Render all nodes at top-level
		for (auto& node : nodes) {
			drawNode(commandBuffer, pipelineLayout, node);
		}
	}

	void Model::createVertexBuffer(const PhysicalDevice* physicalDevice)
	{
		VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

		std::unique_ptr<Buffer> stagingBuffer{ std::make_unique<Buffer>(
			m_logicalDevice,
			physicalDevice,
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) };

		void* data;
		vkMapMemory(*m_logicalDevice, stagingBuffer->getBufferMemory(), 0, bufferSize, 0, &data);
		memcpy(data, m_vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(*m_logicalDevice, stagingBuffer->getBufferMemory());

		m_vertexBuffer = std::make_unique<Buffer>(
			m_logicalDevice,
			physicalDevice,
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_vertexBuffer->copyBuffer(stagingBuffer.get(), bufferSize);

	}

	void Model::createIndexBuffer(const PhysicalDevice* physicalDevice)
	{
		VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

		std::unique_ptr<Buffer> stagingBuffer{ std::make_unique<Buffer>(
			m_logicalDevice,
			physicalDevice,
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) };

		void* data;
		vkMapMemory(*m_logicalDevice, stagingBuffer->getBufferMemory(), 0, bufferSize, 0, &data);
		memcpy(data, m_indices.data(), (size_t)bufferSize);
		vkUnmapMemory(*m_logicalDevice, stagingBuffer->getBufferMemory());

		m_indexBuffer = std::make_unique<Buffer>(
			m_logicalDevice, 
			physicalDevice, 
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_indexBuffer->copyBuffer(stagingBuffer.get(), bufferSize);
	}


	void Model::drawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, Node node)
	{
		if (node.mesh.primitives.size() > 0) {
			// Pass the node's matrix via push constants
			// Traverse the node hierarchy to the top-most parent to get the final matrix of the current node
			glm::mat4 nodeMatrix = node.matrix;
			Node* currentParent = node.parent;
			while (currentParent) {
				nodeMatrix = currentParent->matrix * nodeMatrix;
				currentParent = currentParent->parent;
			}
			// Pass the final matrix to the vertex shader using push constants
			//vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &nodeMatrix);
			for (Primitive& primitive : node.mesh.primitives) {
				if (primitive.indexCount > 0) {
					// Get the texture index for this primitive
					//Texture texture = textures[materials[primitive.materialIndex].baseColorTextureIndex];
					// Bind the descriptor for the current primitive's texture
					//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &images[texture.imageIndex].descriptorSet, 0, nullptr);
					//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &m_descriptorSets[index], 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, primitive.indexCount, 1, primitive.firstIndex, 0, 0);
				}
			}
		}
		for (auto& child : node.children) {
			drawNode(commandBuffer, pipelineLayout, child);
		}
	}

	void Model::createTexture(const PhysicalDevice* physicalDevice, std::string texturePath)
	{
		int texWidth;
		int texHeight;
		int texChannels;
		stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels)
		{
			throw std::runtime_error("failed to load texture image!");
		}

		std::unique_ptr<Buffer> stagingBuffer{ std::make_unique<Buffer>(
			m_logicalDevice,
			physicalDevice,
			imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) };

		void* data;
		vkMapMemory(*m_logicalDevice, stagingBuffer->getBufferMemory(), 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(*m_logicalDevice, stagingBuffer->getBufferMemory());

		stbi_image_free(pixels);

		m_texture = std::make_unique<Image>(
			m_logicalDevice, 
			physicalDevice, 
			texWidth, 
			texHeight, 
			VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT);

		m_texture->transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		m_texture->copyFromBuffer(*stagingBuffer, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		m_texture->transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}
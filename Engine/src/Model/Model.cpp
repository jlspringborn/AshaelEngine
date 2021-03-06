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
		std::string modelPath) :
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
			drawNode(commandBuffer, pipelineLayout, *node);
		}
	}

	void Model::createVertexBuffer(const PhysicalDevice* physicalDevice)
	{
		VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

		m_vertexBuffer = Buffer::createDeviceLocalBuffer(
			m_logicalDevice, 
			physicalDevice, 
			bufferSize, 
			m_vertices.data(), 
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	}

	void Model::createIndexBuffer(const PhysicalDevice* physicalDevice)
	{
		VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

		m_indexBuffer = Buffer::createDeviceLocalBuffer(
			m_logicalDevice,
			physicalDevice,
			bufferSize,
			m_indices.data(),
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
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
					Texture texture = m_textures[m_materials[primitive.materialIndex].baseColorTextureIndex];
					// Bind the descriptor for the current primitive's texture
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &m_textureImages[texture.imageIndex].descriptorSet, 0, nullptr);
					vkCmdDrawIndexed(commandBuffer, primitive.indexCount, 1, primitive.firstIndex, 0, 0);
				}
			}
		}
		for (auto& child : node.children) {
			drawNode(commandBuffer, pipelineLayout, *child);
		}
	}

	void Model::createDescriptorSets(VkDescriptorPool pool, VkDescriptorSetLayout layout, VkSampler sampler)
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		for (auto& image : m_textureImages)
		{
			if (vkAllocateDescriptorSets(*m_logicalDevice, &allocInfo, &image.descriptorSet) != VK_SUCCESS)
			{
				throw std::runtime_error("railed to allocate descriptor sets!");
			}

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = *image.texture;	// * returns image view
			imageInfo.sampler = sampler;

			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = image.descriptorSet;
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(*m_logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}


	}

	glm::mat4 Model::getNodeMatrix(Node* node)
	{
		glm::mat4	nodeMatrix		{ node->getLocalMatrix() };
		Node*		currentParent	{ node->parent };

		while (currentParent)
		{
			nodeMatrix		= currentParent->getLocalMatrix() * nodeMatrix;
			currentParent	= currentParent->parent;
		}
		return nodeMatrix;
	}

	void Model::updateJoints(Node* node)
	{
		if (node->skin > -1)
		{
			// update the joint matrices
			glm::mat4 inverseTransform{ glm::inverse(getNodeMatrix(node)) };
			Skin& skin = m_skins[node->skin];
			size_t numJoints{ (uint32_t)skin.joints.size() };
			std::vector<glm::mat4> jointMatrices(numJoints);
			for (size_t i = 0; i < numJoints; i++)
			{
				jointMatrices[i] = getNodeMatrix(skin.joints[i]) * skin.inverseBindMatrices[i];
				jointMatrices[i] = inverseTransform * jointMatrices[i];
			}

			// update ssbo
			skin.ssbo->copyTo(jointMatrices.data(), jointMatrices.size() * sizeof(glm::mat4));
		}
		
		for (auto& child : node->children)
		{
			updateJoints(child);
		}
	}

	void Model::updateAnimation(float deltaTime)
	{
		if (m_activeAnimation > static_cast<uint32_t>(m_animations.size()) - 1)
		{
			std::cout << "No animation with index " << m_activeAnimation << '\n';
		}
		Animation& animation{ m_animations[m_activeAnimation] };
		animation.currentTime += deltaTime;
		if (animation.currentTime > animation.end)
		{
			animation.currentTime -= animation.end;
		}

		for (auto& channel : animation.channels)
		{
			AnimationSampler& sampler{ animation.samplers[channel.samplerIndex] };
			for (size_t i{0}; i < sampler.inputs.size() - 1; ++i)
			{
				if (sampler.interpolation != "LINEAR")
				{
					std::cout << "Only linear interpolation is currently supported!" << '\n';
					continue;
				}

				// get the input keyframe values for the current time stamp
				if ((animation.currentTime >= sampler.inputs[i]) && (animation.currentTime <= sampler.inputs[i + 1]))
				{
					float a{ animation.currentTime - sampler.inputs[i] / (sampler.inputs[i + 1] - sampler.inputs[i]) };
					if (channel.path == "translation")
					{
						channel.node->translation = glm::mix(sampler.outputsVec4[i], sampler.outputsVec4[i + 1], a);
					}

					if (channel.path == "rotation")
					{
						glm::quat q1;
						q1.x = sampler.outputsVec4[i].x;
						q1.y = sampler.outputsVec4[i].y;
						q1.z = sampler.outputsVec4[i].z;
						q1.w = sampler.outputsVec4[i].w;

						glm::quat q2;
						q2.x = sampler.outputsVec4[i + 1].x;
						q2.y = sampler.outputsVec4[i + 1].y;
						q2.z = sampler.outputsVec4[i + 1].z;
						q2.w = sampler.outputsVec4[i + 1].w;

						channel.node->rotation = glm::normalize(glm::slerp(q1, q2, a));
					}
					if (channel.path == "scale")
					{
						channel.node->scale = glm::mix(sampler.outputsVec4[i], sampler.outputsVec4[i + 1], a);
					}
				}
			}
		}
		for (auto& node : nodes)
		{
			updateJoints(node);
		}
	}

	// NOT CURRENTLY USED: textures are now loaded directly from glTF files
	
	//void Model::createTexture(const PhysicalDevice* physicalDevice, std::string texturePath)
	//{
	//	int texWidth;
	//	int texHeight;
	//	int texChannels;
	//	stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	//	VkDeviceSize imageSize = texWidth * texHeight * 4;

	//	if (!pixels)
	//	{
	//		throw std::runtime_error("failed to load texture image!");
	//	}

	//	std::unique_ptr<Buffer> stagingBuffer{ std::make_unique<Buffer>(
	//		m_logicalDevice,
	//		physicalDevice,
	//		imageSize,
	//		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
	//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) };

	//	void* data;
	//	vkMapMemory(*m_logicalDevice, stagingBuffer->getBufferMemory(), 0, imageSize, 0, &data);
	//	memcpy(data, pixels, static_cast<size_t>(imageSize));
	//	vkUnmapMemory(*m_logicalDevice, stagingBuffer->getBufferMemory());

	//	stbi_image_free(pixels);

	//	m_texture = std::make_unique<Image>(
	//		m_logicalDevice, 
	//		physicalDevice, 
	//		texWidth, 
	//		texHeight, 
	//		VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
	//		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	//		VK_IMAGE_ASPECT_COLOR_BIT);

	//	m_texture->transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	//	m_texture->copyFromBuffer(*stagingBuffer, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	//	m_texture->transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	//}

	glm::mat4 Node::getLocalMatrix()
	{
		return glm::translate(glm::mat4(1.0f), translation) 
			* glm::mat4(rotation) 
			* glm::scale(glm::mat4(1.0f), scale) 
			* matrix;
	}
}
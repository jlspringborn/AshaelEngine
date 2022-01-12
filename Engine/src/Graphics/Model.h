/**
 * Drawable 3D object
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"
#include "Vulkan/Buffer.h"
#include "Vulkan/Image.h"
#include "Vulkan/Vertex.hpp"
#include "Vulkan/PushConstantData.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>

#include <chrono>
#include <vector>
#include <array>
#include <string>

namespace ash
{

	// TODO: Move this to another script
	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class Model
	{
	public:
		Model(const LogicalDevice* logicalDevice, const PhysicalDevice* physicalDevice,
			const int swapChainImageCount, VkDescriptorSetLayout setLayout, VkDescriptorPool pool,
			VkSampler sampler, std::vector<std::unique_ptr<Buffer>>& uniformBuffers);
		~Model();

		/**
		 * Binds buffers and descriptor set, then calls draw command
		 */
		void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index);

		/**
		 * Creates buffer to hold vertices
		 */
		void createVertexBuffer(const PhysicalDevice* physicalDevice);

		/**
		 * Creates buffer to hold indices of vertices
		 */
		void createIndexBuffer(const PhysicalDevice* physicalDevice);

		/**
		 * Creates texture image to display on geometry
		 */
		void createTexture(const PhysicalDevice* physicalDevice);

		/**
		 * Creates a descriptor set for each swap chain image
		 */
		void createDescriptorSets(const uint32_t swapChainImageCount, VkDescriptorSetLayout setLayout, VkDescriptorPool pool, VkSampler sampler, std::vector<std::unique_ptr<Buffer>>& uniformBuffers);

		void cleanupDescriptorSets();

		void setOffset(glm::vec3 offset);

		const std::vector<VkDescriptorSet>& getDescriptorSets() const { return descriptorSets; }

	private:

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};

		/**
		 * TEMP variables to hold the model path and texture path
		 */
		const std::string m_modelPath = "models/viking_room.obj";
		const std::string m_texturePath = "textures/viking_room.png";

		/**
		 * Vertex data of model
		 */
		std::vector<Vertex> m_vertices;

		/**
		 * Index of each vertex, used for more efficient rendering
		 */
		std::vector<uint32_t> m_indices;

		/**
		 * Buffer to hold vertex data
		 */
		std::unique_ptr<Buffer> m_vertexBuffer;
		
		/**
		 * Buffer to hold index of vertices
		 */
		std::unique_ptr<Buffer> m_indexBuffer;

		/**
		 * Array of Descriptor Sets, one for each Uniform Buffer
		 */
		std::vector<VkDescriptorSet> descriptorSets{};

		std::unique_ptr<Image> m_texture{};

		PushConstantData m_push{};

	};
}

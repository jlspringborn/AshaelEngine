/**
 * Drawable 3D object
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"
#include "Vulkan/Buffer.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

#include <chrono>
#include <vector>
#include <array>

namespace ash
{
	// TODO: Move this to another script
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
			// position
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);
			// color
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);
			return attributeDescriptions;
		}
	};

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
			const int swapChainImageCount, VkDescriptorSetLayout setLayout, VkDescriptorPool pool);
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
		 * Creates a uniform buffer for each swap chain frame to allow for
		 * matrix transformations during shading
		 */
		void createUniformBuffers(const PhysicalDevice* physicalDevice, const int swapChainImageCount);

		/**
		 * Deletes all uniform buffers, called during swap chain recreation
		 */
		void cleanupUniformBuffers();

		/**
		 * Updates uniform buffers with new transform data
		 */
		void updateUniformBuffer(uint32_t currentImage, VkExtent2D extent);

		/**
		 * Creates a descriptor set for each swap chain image
		 */
		void createDescriptorSets(const uint32_t swapChainImageCount, VkDescriptorSetLayout setLayout, VkDescriptorPool pool);

	private:

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};

		/**
		 * TEMP: holds vertex data
		 */
		const std::vector<Vertex> vertices =
		{
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};

		/**
		 * TEMP: holds index data
		 */
		const std::vector<uint16_t> indices =
		{
			0, 1, 2, 2, 3, 0
		};

		/**
		 * Buffer to hold vertex data
		 */
		std::unique_ptr<Buffer> m_vertexBuffer;
		
		/**
		 * Buffer to hold index of vertices
		 */
		std::unique_ptr<Buffer> m_indexBuffer;

		/**
		 * Array of Uniform buffers, one for each swap chain image
		 */
		std::vector<std::unique_ptr<Buffer>> m_uniformBuffers;

		/**
		 * Array of Descriptor Sets, one for each Uniform Buffer
		 */
		std::vector<VkDescriptorSet> descriptorSets{};

	};
}

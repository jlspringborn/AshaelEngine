#pragma once

#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

#include <chrono>
#include <vector>
#include <array>

namespace ash
{
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

		void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index);

		void createVertexBuffer(const PhysicalDevice* physicalDevice);

		void createIndexBuffer(const PhysicalDevice* physicalDevice);

		void createUniformBuffers(const PhysicalDevice* physicalDevice, const int swapChainImageCount);

		void cleanupUniformBuffers();

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
			VkBuffer& buffer, VkDeviceMemory& bufferMemory, const PhysicalDevice* physicalDevice);

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		void updateUniformBuffer(uint32_t currentImage, VkExtent2D extent);

		void createDescriptorSets(const uint32_t swapChainImageCount, VkDescriptorSetLayout setLayout, VkDescriptorPool pool);

	private:

		const LogicalDevice* m_logicalDevice{};

		const std::vector<Vertex> vertices =
		{
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};

		const std::vector<uint16_t> indices =
		{
			0, 1, 2, 2, 3, 0
		};

		VkBuffer vertexBuffer{};
		VkDeviceMemory vertexBufferMemory{};

		VkBuffer indexBuffer{};
		VkDeviceMemory indexBufferMemory{};

		std::vector<VkBuffer> uniformBuffers{};
		std::vector<VkDeviceMemory> uniformBuffersMemory{};

		std::vector<VkDescriptorSet> descriptorSets{};

	};
}

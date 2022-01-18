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
#include "TransformComponent.hpp"


#include <vulkan/vulkan.h>

#include <chrono>
#include <vector>
#include <array>
#include <string>

namespace ash
{
	/**
	 * Drawable 3D object
	 */
	class Model
	{
	public:
		Model(
			const LogicalDevice* logicalDevice, 
			const PhysicalDevice* physicalDevice,
			const int swapChainImageCount, 
			VkDescriptorSetLayout setLayout, 
			VkDescriptorPool pool,
			VkSampler sampler, 
			std::vector<std::unique_ptr<Buffer>>& uniformBuffers,
			std::string modelPath,
			std::string texturePath
		);

		~Model();

		/**
		 * Binds buffers and descriptor set, then calls draw command
		 */
		void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index, TransformComponent* transform);

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
		void createTexture(const PhysicalDevice* physicalDevice, std::string texturePath);

		/**
		 * Creates a descriptor set for each swap chain image
		 */
		void createDescriptorSets(const uint32_t swapChainImageCount, VkDescriptorSetLayout setLayout, VkDescriptorPool pool, VkSampler sampler, std::vector<std::unique_ptr<Buffer>>& uniformBuffers);

		/**
		 * Called during swap chain recreation and class destruction
		 */
		void cleanupDescriptorSets();

	private:

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};

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
		std::vector<VkDescriptorSet> m_descriptorSets{};

		/**
		 * Texture to be displayed on geometry during fragment stage of pipeline
		 */
		std::unique_ptr<Image> m_texture{};

	};
}

#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

namespace ash
{
	Model::Model(const LogicalDevice* logicalDevice, const PhysicalDevice* physicalDevice, 
		const int swapChainImageCount, VkDescriptorSetLayout setLayout, VkDescriptorPool pool,
		VkSampler sampler, VkExtent2D extent) :
		m_logicalDevice{ logicalDevice }
	{
		createDepthResources(physicalDevice, extent);
		createTexture(physicalDevice);
		createVertexBuffer(physicalDevice);
		createIndexBuffer(physicalDevice);
		createUniformBuffers(physicalDevice, swapChainImageCount);
		createDescriptorSets(swapChainImageCount, setLayout, pool, sampler);
	}

	Model::~Model()
	{
	}

	void Model::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index)
	{
		VkBuffer vertexBuffers[] = { *m_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, *m_indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[index], 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	}

	void Model::createVertexBuffer(const PhysicalDevice* physicalDevice)
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		std::unique_ptr<Buffer> stagingBuffer{ std::make_unique<Buffer>(
			m_logicalDevice,
			physicalDevice,
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) };

		void* data;
		vkMapMemory(*m_logicalDevice, stagingBuffer->getBufferMemory(), 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
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
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		std::unique_ptr<Buffer> stagingBuffer{ std::make_unique<Buffer>(
			m_logicalDevice,
			physicalDevice,
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) };

		void* data;
		vkMapMemory(*m_logicalDevice, stagingBuffer->getBufferMemory(), 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(*m_logicalDevice, stagingBuffer->getBufferMemory());

		m_indexBuffer = std::make_unique<Buffer>(
			m_logicalDevice, 
			physicalDevice, 
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_indexBuffer->copyBuffer(stagingBuffer.get(), bufferSize);
	}

	void Model::createUniformBuffers(const PhysicalDevice* physicalDevice, const int swapChainImageCount)
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		
		m_uniformBuffers.resize(swapChainImageCount);

		for (size_t i = 0; i < swapChainImageCount; i++)
		{
			m_uniformBuffers[i] = std::make_unique<Buffer>(m_logicalDevice, 
				physicalDevice, 
				bufferSize, 
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
	}

	void Model::cleanupUniformBuffers()
	{
		size_t bufferCount{ m_uniformBuffers.size() };
		for (size_t i = 0; i < bufferCount; i++)
		{
			m_uniformBuffers[i] = nullptr;
		}
	}

	void Model::updateUniformBuffer(uint32_t currentImage, VkExtent2D extent)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.f);
		ubo.proj[1][1] *= -1;

		void* data;
		vkMapMemory(*m_logicalDevice, m_uniformBuffers[currentImage]->getBufferMemory(), 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(*m_logicalDevice, m_uniformBuffers[currentImage]->getBufferMemory());
	}

	void Model::createDescriptorSets(const uint32_t swapChainImageCount, VkDescriptorSetLayout setLayout, VkDescriptorPool pool, VkSampler sampler)
	{
		std::vector<VkDescriptorSetLayout> layouts(swapChainImageCount, setLayout);

		VkDescriptorSetAllocateInfo allociInfo{};
		allociInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allociInfo.descriptorPool = pool;
		allociInfo.descriptorSetCount = swapChainImageCount;
		allociInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(swapChainImageCount);
		if (vkAllocateDescriptorSets(*m_logicalDevice, &allociInfo, descriptorSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("railed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < swapChainImageCount; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = *m_uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = *m_texture;	// * returns image view
			imageInfo.sampler = sampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			// uniform buffer
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			// image sampler
			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;
			

			vkUpdateDescriptorSets(*m_logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void Model::createTexture(const PhysicalDevice* physicalDevice)
	{
		int texWidth;
		int texHeight;
		int texChannels;
		stbi_uc* pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

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

	void Model::createDepthResources(const PhysicalDevice* phyiscalDevice, VkExtent2D extent)
	{
		VkFormat depthFormat = findDepthFormat(phyiscalDevice);

		m_depthImage = std::make_unique<Image>(m_logicalDevice,
			phyiscalDevice,
			extent.width,
			extent.height,
			depthFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	VkFormat Model::findDepthFormat(const PhysicalDevice* phyiscalDevice)
	{
		return phyiscalDevice->findSupportedFormat(
			{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	bool Model::hasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}
}
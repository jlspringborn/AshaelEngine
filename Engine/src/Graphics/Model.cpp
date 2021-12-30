#include "Model.h"

#include <stdexcept>

namespace ash
{
	Model::Model(const LogicalDevice* logicalDevice, const PhysicalDevice* physicalDevice, const int swapChainImageCount, VkDescriptorSetLayout setLayout, VkDescriptorPool pool) :
		m_logicalDevice{ logicalDevice }
	{
		createVertexBuffer(physicalDevice);
		createIndexBuffer(physicalDevice);
		createUniformBuffers(physicalDevice, swapChainImageCount);
		createDescriptorSets(swapChainImageCount, setLayout, pool);
	}

	Model::~Model()
	{
		cleanupUniformBuffers();

		vkDestroyBuffer(*m_logicalDevice, indexBuffer, nullptr);
		vkFreeMemory(*m_logicalDevice, indexBufferMemory, nullptr);

		vkDestroyBuffer(*m_logicalDevice, vertexBuffer, nullptr);
		vkFreeMemory(*m_logicalDevice, vertexBufferMemory, nullptr);
	}

	void Model::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index)
	{
		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[index], 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	}

	void Model::createVertexBuffer(const PhysicalDevice* physicalDevice)
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 
			| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, physicalDevice);

		void* data;
		vkMapMemory(*m_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(*m_logicalDevice, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory, physicalDevice);

		copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(*m_logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(*m_logicalDevice, stagingBufferMemory, nullptr);
	}

	void Model::createIndexBuffer(const PhysicalDevice* physicalDevice)
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, physicalDevice);

		void* data;
		vkMapMemory(*m_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(*m_logicalDevice, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory, physicalDevice);

		copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(*m_logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(*m_logicalDevice, stagingBufferMemory, nullptr);
	}

	void Model::createUniformBuffers(const PhysicalDevice* physicalDevice, const int swapChainImageCount)
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		
		uniformBuffers.resize(swapChainImageCount);
		uniformBuffersMemory.resize(swapChainImageCount);

		for (size_t i = 0; i < swapChainImageCount; i++)
		{
			createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i], physicalDevice);
		}
	}

	void Model::cleanupUniformBuffers()
	{
		size_t bufferCount{ uniformBuffers.size() };
		for (size_t i = 0; i < bufferCount; i++)
		{
			vkDestroyBuffer(*m_logicalDevice, uniformBuffers[i], nullptr);
			vkFreeMemory(*m_logicalDevice, uniformBuffersMemory[i], nullptr);
		}
	}

	void Model::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, const PhysicalDevice* physicalDevice)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(*m_logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(*m_logicalDevice, buffer, &memRequirements);
		
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = physicalDevice->findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(*m_logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(*m_logicalDevice, buffer, bufferMemory, 0);
	}

	void Model::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_logicalDevice->getCommandPool();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(*m_logicalDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;

		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		vkQueueSubmit(m_logicalDevice->getGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_logicalDevice->getGraphicQueue());
		vkFreeCommandBuffers(*m_logicalDevice, m_logicalDevice->getCommandPool(), 1, &commandBuffer);
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
		vkMapMemory(*m_logicalDevice, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(*m_logicalDevice, uniformBuffersMemory[currentImage]);
	}

	void Model::createDescriptorSets(const uint32_t swapChainImageCount, VkDescriptorSetLayout setLayout, VkDescriptorPool pool)
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
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(*m_logicalDevice, 1, &descriptorWrite, 0, nullptr);
		}
	}
}
/**
 * Copyright (C) 2021, Jesse Springborn
 */
#include "Vulkan/Buffer.h"

#include <stdexcept>
#include <memory>

namespace ash
{
	Buffer::Buffer(const LogicalDevice* logicalDevice, const PhysicalDevice* physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) :
		m_logicalDevice{ logicalDevice }
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(*m_logicalDevice, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(*m_logicalDevice, m_buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = physicalDevice->findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(*m_logicalDevice, &allocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(*m_logicalDevice, m_buffer, m_bufferMemory, 0);
	}

	Buffer::~Buffer()
	{
		vkDestroyBuffer(*m_logicalDevice, m_buffer, nullptr);
		vkFreeMemory(*m_logicalDevice, m_bufferMemory, nullptr);
	}

	void Buffer::copyBuffer(const Buffer* srcBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = m_logicalDevice->beginSingleTimeCommand();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;

		vkCmdCopyBuffer(commandBuffer, *srcBuffer, m_buffer, 1, &copyRegion);
		
		m_logicalDevice->endSingleTimeCommand(commandBuffer);
	}

	std::unique_ptr<Buffer> Buffer::createDeviceLocalBuffer(
		const LogicalDevice* logicalDevice, 
		const PhysicalDevice* physicalDevice, 
		VkDeviceSize bufferSize, 
		const void* inData, 
		VkBufferUsageFlagBits usage)
	{
		// create staging buffer for transfer operation
		std::unique_ptr<Buffer> stagingBuffer{ std::make_unique<Buffer>(
		logicalDevice,
		physicalDevice,
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) };

		// directly copy the data into device coherent memory
		void* data;
		vkMapMemory(*logicalDevice, stagingBuffer->getBufferMemory(), 0, bufferSize, 0, &data);	// 
		memcpy(data, inData, (size_t)bufferSize);
		vkUnmapMemory(*logicalDevice, stagingBuffer->getBufferMemory());

		// create the buffer that will you device local memory
		std::unique_ptr<Buffer> localBuffer = std::make_unique<Buffer>(
			logicalDevice,
			physicalDevice,
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// copy the data from the host coherent buffer to the device local buffer
		localBuffer->copyBuffer(stagingBuffer.get(), bufferSize);

		// return the smart ptr for the local buffer
		return std::move(localBuffer);
	}
}
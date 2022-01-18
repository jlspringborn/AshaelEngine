/**
 * Copyright (C) 2021, Jesse Springborn
 */
#include "Vulkan/Buffer.h"

#include <stdexcept>

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
}
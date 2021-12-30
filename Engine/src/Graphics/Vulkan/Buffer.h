/**
 * Wrapper class for a Vulkan buffer
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Vulkan\PhysicalDevice.h"
#include "Vulkan\LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace ash
{
	/**
	 * Wrapper class for Vulkan Buffer
	 */
	class Buffer
	{
	public:
		Buffer(const LogicalDevice* logicalDevice, const PhysicalDevice* physicalDevice, 
			VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
		~Buffer();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkBuffer& () const { return m_buffer; }

		const VkDeviceMemory& getBufferMemory() const { return m_bufferMemory; }

		/**
		 * Copies the data from the provided buffer into self
		 * @param buffer to copy from
		 * @param size of the source buffer
		 */
		void copyBuffer(const Buffer* srcBuffer, VkDeviceSize size);

	private:

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};

		/**
		 * Vulkan Buffer
		 */
		VkBuffer m_buffer{};

		/**
		 * Memory used by Vulkan Buffer
		 */
		VkDeviceMemory m_bufferMemory{};

	};
}
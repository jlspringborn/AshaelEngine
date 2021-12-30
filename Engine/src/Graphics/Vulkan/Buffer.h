#pragma once

#include "Vulkan\PhysicalDevice.h"
#include "Vulkan\LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace ash
{
	class Buffer
	{
	public:
		Buffer(const LogicalDevice* logicalDevice, const PhysicalDevice* physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
		~Buffer();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkBuffer& () const { return m_buffer; }

		const VkDeviceMemory& getBufferMemory() const { return m_bufferMemory; }

		void copyBuffer(const Buffer* srcBuffer, VkDeviceSize size);

	private:

		const LogicalDevice* m_logicalDevice{};

		VkBuffer m_buffer{};
		VkDeviceMemory m_bufferMemory{};

	};
}
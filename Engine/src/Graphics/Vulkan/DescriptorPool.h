#pragma once

#include "Vulkan/LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace ash
{
	class DescriptorPool
	{
	public:
		DescriptorPool(const LogicalDevice* logicalDevice, const uint32_t swapChainImagecount);
		~DescriptorPool();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkDescriptorPool& () const { return m_descriptorPool; }

		void createDescriptorPool(const uint32_t swapChainImagecount);

		void cleanupDescriptorPool();

	private:

		VkDescriptorPool m_descriptorPool{};

		const LogicalDevice* m_logicalDevice{};
	};
}
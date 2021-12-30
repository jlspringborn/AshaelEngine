/**
 * Wrapper for Vulkan Graphics Pipeline
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Vulkan/LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace ash
{
	/**
	 * Wrapper for Vulkan Descriptor Pool
	 */
	class DescriptorPool
	{
	public:
		DescriptorPool(const LogicalDevice* logicalDevice, const uint32_t swapChainImagecount);
		~DescriptorPool();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkDescriptorPool& () const { return m_descriptorPool; }

		/**
		 * Creates Descriptor Pool, sets max values based on swap chain image count
		 */
		void createDescriptorPool(const uint32_t swapChainImagecount);

		/**
		 * Deletes Descriptor Pool, called during swap chain recreation
		 */
		void cleanupDescriptorPool();

	private:

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};

		/**
		 * Vulkan Descriptor Pool
		 */
		VkDescriptorPool m_descriptorPool{};
	};
}
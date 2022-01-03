/**
 * Wrapper for Vulkan Logical Device
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Vulkan\Instance.h"
#include "Vulkan\PhysicalDevice.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace ash
{
	/**
	 * Wrapper for Vulkan Logical Device
	 */
	class LogicalDevice
	{
	public:

		LogicalDevice(const Instance* instance, const PhysicalDevice* physicalDevice);

		~LogicalDevice();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkDevice& () const { return m_device; }
		
		/**
		 * Returns Logical Device, use * operator instead
		 */
		const VkDevice& getDevice() const { return m_device; }

		/**
		 * Returns Graphics queue, used for command submission
		 */
		const VkQueue& getGraphicQueue() const { return m_graphicsQueue; }

		/**
		 * Returns Present Queue, used for command submission
		 */
		const VkQueue& getPresentQueue() const { return m_presentQueue; }

		/**
		 * Returns Command Pool, used for command submission
		 */
		const VkCommandPool& getCommandPool() const { return m_commandPool; }

		const VkCommandBuffer beginSingleTimeCommand() const;

		const void endSingleTimeCommand(VkCommandBuffer commandBuffer) const;


	private:
		
		/**
		 * Vulkan Logical Device
		 */
		VkDevice m_device{};

		/**
		 * Vulkan Queue, used for Graphics related commands
		 */
		VkQueue m_graphicsQueue{};

		/**
		 * Vulkan Queue, used for Presentation related commands
		 */
		VkQueue m_presentQueue{};

		/**
		 * Vulkan Command Pool, used for command submission
		 */
		VkCommandPool m_commandPool{};

		/**
		 * Create Vulkan Command Pool
		 */
		void createCommandPool(const PhysicalDevice* physicalDevice);
	};
}
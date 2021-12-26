#pragma once

#include "Vulkan\Instance.h"
#include "Vulkan\PhysicalDevice.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace ash
{
	class LogicalDevice
	{
	public:

		LogicalDevice(const Instance* instance, const PhysicalDevice* physicalDevice);

		~LogicalDevice();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkDevice& () const { return m_device; }

		const VkDevice& getDevice() const { return m_device; }

		const VkQueue& getGraphicQueue() const { return m_graphicsQueue; }

		const VkQueue& getPresentQueue() const { return m_presentQueue; }

		const VkCommandPool& getCommandPool() const { return m_commandPool; }

	private:

		VkDevice m_device{};

		VkQueue m_graphicsQueue{};

		VkQueue m_presentQueue{};

		VkCommandPool m_commandPool{};

		void createCommandPool(const PhysicalDevice* physicalDevice);
	};
}
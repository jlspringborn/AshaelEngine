#include "Vulkan\LogicalDevice.h"

#include <stdexcept>
#include <vector>
#include <set>

namespace ash
{
	LogicalDevice::LogicalDevice(const Instance* instance, const PhysicalDevice* physicalDevice)
	{
		PhysicalDevice::QueueFamilyIndices indices = physicalDevice->getQueueFamilyIndices();

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;

		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex	= queueFamily;
			queueCreateInfo.queueCount			= 1;
			queueCreateInfo.pQueuePriorities	= &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}


		VkDeviceCreateInfo createInfo{};
		createInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos		= queueCreateInfos.data();
		createInfo.queueCreateInfoCount		= static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pEnabledFeatures			= &(physicalDevice->getDeviceFeatures());
		createInfo.enabledExtensionCount	= static_cast<uint32_t>(physicalDevice->getDeviceExtensions().size());
		createInfo.ppEnabledExtensionNames	= physicalDevice->getDeviceExtensions().data();

		if (instance->isValidationEnabled)
		{
			createInfo.enabledLayerCount	= static_cast<uint32_t>(instance->getValidationLayers().size());
			createInfo.ppEnabledLayerNames	= instance->getValidationLayers().data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}
		if (vkCreateDevice(*physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(m_device, physicalDevice->getQueueFamilyIndices().graphicsFamily.value(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, physicalDevice->getQueueFamilyIndices().presentFamily.value(), 0, &m_presentQueue);

		createCommandPool(physicalDevice);
	}

	LogicalDevice::~LogicalDevice()
	{
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);
		vkDestroyDevice(m_device, nullptr);
	}

	void LogicalDevice::createCommandPool(const PhysicalDevice* physicalDevice)
	{
		const PhysicalDevice::QueueFamilyIndices& queueFamilyIndices = physicalDevice->getQueueFamilyIndices();

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		poolInfo.flags = 0;

		if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}
	}
}
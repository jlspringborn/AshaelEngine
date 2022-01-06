#include "Vulkan\PhysicalDevice.h"

#include <stdexcept>
#include <vector>
#include <set>
#include <string>

namespace ash
{
	PhysicalDevice::PhysicalDevice(const Instance* instance, const Surface* surface) :
		m_surface{ surface }
	{
		uint32_t deviceCount{ 0 };
		vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (isDeviceSuitable(device, *surface))
			{
				m_physicalDevice = device;
				break;
			}
		}

		if (m_physicalDevice == VK_NULL_HANDLE)
		{
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);
		vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_features);
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memoryProperties);
		queueFamilyIndices = findQueueFamilies(m_physicalDevice, *surface);
		swapChainsSupportDetails = querySwapChainSupport(m_physicalDevice, *surface);
	}

	PhysicalDevice::~PhysicalDevice()
	{
	}

	const uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ( (typeFilter & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties) )
			{
				return i;
			}
		}
		
		throw std::runtime_error("failed to find suitable memory type!");
	}

	const VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &properties);
			if ( (tiling == VK_IMAGE_TILING_LINEAR) && ((properties.linearTilingFeatures & features) == features) )
			{
				return format;
			}
			else if ( (tiling == VK_IMAGE_TILING_OPTIMAL) && ((properties.optimalTilingFeatures & features) == features) )
			{
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}

	bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices = findQueueFamilies(device, surface);

		bool bExtensionsSupported = checkDeviceExtensionSupport(device);

		bool bSwapChainAdequate{ false };
		if (bExtensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
			bSwapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.isComplete() && bExtensionsSupported && bSwapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	PhysicalDevice::QueueFamilyIndices PhysicalDevice::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int index{ 0 };
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = index;
			}

			VkBool32 bPresentSupport{ false };
			vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &bPresentSupport);

			if (bPresentSupport)
			{
				indices.presentFamily = index;
			}

			if (indices.isComplete())
			{
				break;
			}

			++index;
		}

		return indices;
	}

	bool PhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);	// erase extensions when found
		}

		return requiredExtensions.empty();	// if all empty all extensions were found
	}

	PhysicalDevice::SwapChainSupportDetails PhysicalDevice::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
}
/**
* manages vulkan physical device
*
* Copyright (C) 2021, Jesse Springborn
*/
#pragma once

#include "Vulkan\Instance.h"
#include "Vulkan\Surface.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace ash
{
	class PhysicalDevice
	{
	public:
		/**
		 * for detecting if GPU contains required queues
		 */
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete()
			{
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

	public:
		PhysicalDevice(const Instance* instance, const Surface* surface);
		~PhysicalDevice();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkPhysicalDevice& () const { return m_physicalDevice; }

		const VkPhysicalDevice& getPhysicalDevice() const { return m_physicalDevice; }

		const VkPhysicalDeviceProperties& getProperties() const { return m_properties; }

		const VkPhysicalDeviceFeatures& getDeviceFeatures() const { return m_features; }

		const VkPhysicalDeviceMemoryProperties& getMemoryProperties() const { return m_memoryProperties; }

		const VkSampleCountFlagBits& getMsaaSamples() const { return m_msaaSamples; }

		const QueueFamilyIndices& getQueueFamilyIndices() const { return queueFamilyIndices; }

		const std::vector<const char*>& getDeviceExtensions() const { return deviceExtensions; }

		const SwapChainSupportDetails getSwapChainSupportDetails() const { return querySwapChainSupport(m_physicalDevice, *m_surface); }

	private:

		const Surface* m_surface{};

		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

		VkPhysicalDeviceProperties m_properties{};	// TODO: not currently used

		VkPhysicalDeviceFeatures m_features{};		// TODO: not currently used

		VkPhysicalDeviceMemoryProperties m_memoryProperties{};

		VkSampleCountFlagBits m_msaaSamples{};

		QueueFamilyIndices queueFamilyIndices{};

		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		SwapChainSupportDetails swapChainsSupportDetails{};

		bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

		bool checkDeviceExtensionSupport(VkPhysicalDevice device);

		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const;
	};
}
/**
* Wrapper for Vulkan Physical Device
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
	/**
	 * Wrapper for Vulkan Physical Device
	 */
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

		/**
		 * Returns Vulkan Physical Device, use * operator instead
		 */
		const VkPhysicalDevice& getPhysicalDevice() const { return m_physicalDevice; }

		/**
		 * Returns struct containing Physical Device properties
		 */
		const VkPhysicalDeviceProperties& getProperties() const { return m_properties; }

		/**
		 * Returns struct containing Physical Device features
		 */
		const VkPhysicalDeviceFeatures& getDeviceFeatures() const { return m_features; }

		/**
		 * Returns struct containing Physical Device memory properties
		 */
		const VkPhysicalDeviceMemoryProperties& getMemoryProperties() const { return m_memoryProperties; }

		/**
		 * NOT CURRENTLY IMPLEMENTED
		 */
		const VkSampleCountFlagBits& getMsaaSamples() const { return m_msaaSamples; }

		/**
		 * Returns struct of containing indices of device Queues
		 */
		const QueueFamilyIndices& getQueueFamilyIndices() const { return queueFamilyIndices; }

		/**
		 * Returns struct containing the device's supported extensions
		 */
		const std::vector<const char*>& getDeviceExtensions() const { return deviceExtensions; }

		/**
		 * Returns struct of swap chain support info for current Physical Device
		 */
		const SwapChainSupportDetails getSwapChainSupportDetails() const { return querySwapChainSupport(m_physicalDevice, *m_surface); }

		/**
		 * Returns index of memory type that fits requested requirements
		 */
		const uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	private:

		/**
		 * Vulkan Physical Device
		 */
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		
		const Surface* m_surface{};

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
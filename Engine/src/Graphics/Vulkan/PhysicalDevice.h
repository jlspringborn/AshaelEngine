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

		/**
		 * Struct that contains swap chain support info of GPU
		 */
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

		/**
		 * Finds and returns the supported depth format of the GPU
		 */
		const VkFormat findDepthFormat() const;

		/**
		 * Returns format supported by GPU, used for depth image creation
		 */
		const VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

	private:

		/**
		 * Vulkan Physical Device
		 */
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		
		/**
		 * Surface used to write images to the screen, needed for config info
		 */
		const Surface* m_surface{};

		/**
		 * Contains GPU supported properties
		 */
		VkPhysicalDeviceProperties m_properties{};	

		/**
		 * Contains GPU supported features
		 */
		VkPhysicalDeviceFeatures m_features{};		

		/**
		 * Contains GPU supported memory properties
		 */
		VkPhysicalDeviceMemoryProperties m_memoryProperties{};

		/**
		 * Contains GPU supported MSAA sample count
		 */
		VkSampleCountFlagBits m_msaaSamples{};

		/**
		 * Contains indices of GPU queue families
		 */
		QueueFamilyIndices queueFamilyIndices{};

		/**
		 * Required extensions to look for when selecting a GPU
		 */
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		/**
		 * Struct containing swap chain support info
		 */
		SwapChainSupportDetails swapChainsSupportDetails{};

		/**
		 * Returns true if the provided GPU has all required features and properties
		 */
		bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

		/**
		 * Find queue family indices of provided GPU
		 */
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

		/**
		 * Returns true if provided GPU has required extensions
		 */
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);

		/**
		 * Returns swap chain support info of provided GPU
		 */
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const;
	};
}
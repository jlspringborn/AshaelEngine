/**
* Wrapper for Vulkan Swap Chain
*
* Copyright (C) 2022, Jesse Springborn
*/
#pragma once

#include "Window.h"
#include "Vulkan/Surface.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace ash
{
	/**
	 * Wrapper for Vulkan Swap Chain
	 */
	class SwapChain
	{
	public:
		SwapChain(const Window* window, const Surface* surface, const PhysicalDevice* physicalDevice, const LogicalDevice* logicalDevice);
		~SwapChain();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkSwapchainKHR& () const { return m_swapChain; }

		/**
		 * Returns reference to swap chain image format
		 */
		const VkFormat& getImageFormat() const { return m_imageFormat; }

		/**
		 * returns reference to the current swap chain image extent
		 */
		const VkExtent2D& getSwapExtent() const { return m_swapExtent; }

		/**
		 * Returns current count of swap chain images
		 */
		const uint32_t getImageCount() const { return m_imageCount; }

		/**
		 * Returns array of swap chain framebuffers
		 */
		const std::vector<VkFramebuffer>& getFramebuffers() const { return m_framebuffers; }

		/**
		 * Creates swap chain framebuffers
		 */
		void createFramebuffers(VkRenderPass renderPass, VkImageView depthImageView);

		/**
		 * Cleans up swap chain framebuffers, called during swap chain recreation
		 */
		void cleanupFramebuffers();

		/**
		 * Cleans up swap chain image views, called during swap chain recreation
		 */
		void cleanupImageViews();

		/**
		 * Cleans up swap chain, called during swap chain recreation
		 */
		void cleanupSwapChain();

		/**
		 * Creates the Vulkan Swap Chain
		 */
		void createSwapChain(const Window* window, const Surface* surface, const PhysicalDevice* physicalDevice);

		/**
		 * Creates image views for all swap chain images
		 */
		void createImageViews();

		/**
		 * Returns the aspect ration of the current swap extent
		 */
		float getAspectRatio() 
		{
			return static_cast<float>(m_swapExtent.width) / static_cast<float>(m_swapExtent.height);
		}

	private:

		/**
		 * Vulkan Swap Chain, manages which image is currently being written to
		 * and which image is currently being presented to the screen
		 */
		VkSwapchainKHR m_swapChain{};

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};

		/**
		 * The image format of the swap chain images
		 */
		VkFormat m_imageFormat{};

		/**
		 * The current extent of the swap chain images, matches window extent
		 */
		VkExtent2D m_swapExtent{};

		/**
		 * The count of swap chain images, will be 2-3 based on whether double buffering
		 * or triple buffering is enabled
		 */
		uint32_t m_imageCount{};

		/**
		 * Array of swap chain images
		 */
		std::vector<VkImage> m_images{};

		/**
		 * Array of swap chain image views, needed to access image content
		 */
		std::vector<VkImageView> m_imageViews{};

		/**
		 * Array of Vulkan framebuffers, will be 2-3 based on whether double buffering
		 * or triple buffering is enabled
		 */
		std::vector<VkFramebuffer> m_framebuffers{};

		/**
		 * Choose swap surface format from list of available formats
		 */
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		/**
		 * Choose swap present mode, this is where we decide whether or not to use double or triple buffering
		 */
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		/**
		 * Choose swap extent, should always match window extent unless using high dpi display
		 */
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilties, const Window* window);




	};
}

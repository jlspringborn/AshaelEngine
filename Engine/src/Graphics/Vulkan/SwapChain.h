#pragma once

#include "Window.h"
#include "Vulkan/Surface.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace ash
{
	class SwapChain
	{
	public:
		SwapChain(const Window* window, const Surface* surface, const PhysicalDevice* physicalDevice, const LogicalDevice* logicalDevice);
		~SwapChain();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkSwapchainKHR& () const { return m_swapChain; }

		const VkFormat& getImageFormat() const { return m_imageFormat; }

		const VkExtent2D& getSwapExtent() const { return m_swapExtent; }

		const uint32_t getImageCount() const { return m_imageCount; }

		const std::vector<VkFramebuffer>& getFramebuffers() const { return m_framebuffers; }

		void createFramebuffers(VkRenderPass renderPass, VkImageView depthImageView);

		void cleanupFramebuffers();

		void cleanupImageViews();

		void cleanupSwapChain();

		void createSwapChain(const Window* window, const Surface* surface, const PhysicalDevice* physicalDevice);

		void createImageViews();

		float extentAspectRatio() 
		{
			return static_cast<float>(m_swapExtent.width) / static_cast<float>(m_swapExtent.height);
		}

	private:

		VkSwapchainKHR m_swapChain{};

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};

		VkFormat m_imageFormat{};

		VkExtent2D m_swapExtent{};

		uint32_t m_imageCount{};

		std::vector<VkImage> m_images{};

		std::vector<VkImageView> m_imageViews{};

		std::vector<VkFramebuffer> m_framebuffers{};

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilties, const Window* window);




	};
}

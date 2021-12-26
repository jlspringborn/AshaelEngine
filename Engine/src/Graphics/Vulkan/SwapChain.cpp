#include "Vulkan/SwapChain.h"

#include <cstdint>	// for UINT32_MAX
#include <algorithm>	// for std::clamp
#include <stdexcept>

namespace ash
{
	SwapChain::SwapChain(const Window* window, const Surface* surface, const PhysicalDevice* physicalDevice, const LogicalDevice* logicalDevice) :
		m_logicalDevice{ logicalDevice }
	{
		createSwapChain(window, surface, physicalDevice);
		createImageViews();
	}

	SwapChain::~SwapChain()
	{
		cleanupFramebuffers();
		cleanupImageViews();
		cleanupSwapChain();
	}

	VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilties, const Window* window)
	{
		if (capabilties.currentExtent.width != UINT32_MAX)
		{
			return capabilties.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = window->getWindowExtent();

			actualExtent.width = std::clamp(actualExtent.width, capabilties.minImageExtent.width, capabilties.minImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilties.minImageExtent.height, capabilties.minImageExtent.height);

			return actualExtent;
		}

	}

	void SwapChain::createSwapChain(const Window* window, const Surface* surface, const PhysicalDevice* physicalDevice)
	{
		const PhysicalDevice::SwapChainSupportDetails swapChainSupportDetails{ physicalDevice->getSwapChainSupportDetails() };
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupportDetails.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupportDetails.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupportDetails.capabilities, window);

		m_imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;

		if ((swapChainSupportDetails.capabilities.maxImageCount > 0)
			&& (m_imageCount > swapChainSupportDetails.capabilities.maxImageCount))
		{
			m_imageCount = swapChainSupportDetails.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType			= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface			= *surface;
		createInfo.minImageCount	= m_imageCount;
		createInfo.imageFormat		= surfaceFormat.format;
		createInfo.imageColorSpace	= surfaceFormat.colorSpace;
		createInfo.imageExtent		= extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		const PhysicalDevice::QueueFamilyIndices& indices = physicalDevice->getQueueFamilyIndices();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode			= VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount	= 2;
			createInfo.pQueueFamilyIndices		= queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform		= swapChainSupportDetails.capabilities.currentTransform;
		createInfo.compositeAlpha	= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode		= presentMode;
		createInfo.clipped			= VK_TRUE;
		createInfo.oldSwapchain		= VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(*m_logicalDevice, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(*m_logicalDevice, m_swapChain, &m_imageCount, nullptr);
		m_images.resize(m_imageCount);
		vkGetSwapchainImagesKHR(*m_logicalDevice, m_swapChain, &m_imageCount, m_images.data());

		m_imageFormat = surfaceFormat.format;
		m_swapExtent = extent;
	}

	void SwapChain::createImageViews()
	{
		m_imageViews.resize(m_images.size());

		for (size_t i{ 0 }; i < m_images.size(); ++i)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_images[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_imageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			
			if (vkCreateImageView(*m_logicalDevice, &createInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create image views!");
			}
		}
	}

	void SwapChain::createFramebuffers(VkRenderPass renderPass)
	{
		m_framebuffers.resize(m_imageViews.size());

		for (size_t i = 0; i < m_imageViews.size(); i++)
		{
			VkImageView attachments[] = { m_imageViews[i] };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_swapExtent.width;
			framebufferInfo.height = m_swapExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(*m_logicalDevice, &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
		
	}

	void SwapChain::cleanupFramebuffers()
	{
		for (auto framebuffer : m_framebuffers)
		{
			vkDestroyFramebuffer(*m_logicalDevice, framebuffer, nullptr);
		}
	}

	void SwapChain::cleanupImageViews()
	{
		for (auto imageView : m_imageViews)
		{
			vkDestroyImageView(*m_logicalDevice, imageView, nullptr);
		}
	}

	void SwapChain::cleanupSwapChain()
	{
		vkDestroySwapchainKHR(*m_logicalDevice, m_swapChain, nullptr);
	}
}
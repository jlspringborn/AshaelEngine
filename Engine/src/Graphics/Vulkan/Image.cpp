#include "Vulkan/Image.h"

#include <stdexcept>

namespace ash
{
	Image::Image(
		const LogicalDevice* logicalDevice,
		const PhysicalDevice* physicalDevice,
		uint32_t width,
		uint32_t height,
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkImageAspectFlags aspectFlags)
		: m_logicalDevice{ logicalDevice }
	{
		createImage(physicalDevice, width, height, format, tiling, usage, properties);
		createImageView(format, aspectFlags);
	}

	Image::~Image()
	{
		vkDestroyImageView(*m_logicalDevice, m_imageView, nullptr);
		vkDestroyImage(*m_logicalDevice, m_image, nullptr);
		vkFreeMemory(*m_logicalDevice, m_imageMemory, nullptr);
	}

	void Image::createImage(
		const PhysicalDevice* physicalDevice, 
		uint32_t width, 
		uint32_t height, 
		VkFormat format, VkImageTiling tiling, 
		VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties)
	{
		// Image creation info
		VkImageCreateInfo imageInfo{};
		imageInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType		= VK_IMAGE_TYPE_2D;
		imageInfo.extent.width	= width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth	= 1;
		imageInfo.mipLevels		= 1;
		imageInfo.arrayLayers	= 1;
		imageInfo.format		= format;
		imageInfo.tiling		= tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage			= usage;
		imageInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples		= VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags			= 0;

		if (vkCreateImage(*m_logicalDevice, &imageInfo, nullptr, &m_image) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(*m_logicalDevice, m_image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize	= memRequirements.size;
		allocInfo.memoryTypeIndex	= physicalDevice->findMemoryType(
			memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(*m_logicalDevice, &allocInfo, nullptr, &m_imageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(*m_logicalDevice, m_image, m_imageMemory, 0);
	}

	void Image::createImageView(VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image								= m_image;
		viewInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format								= format;
		viewInfo.subresourceRange.aspectMask		= aspectFlags;
		viewInfo.subresourceRange.baseMipLevel		= 0;
		viewInfo.subresourceRange.levelCount		= 1;
		viewInfo.subresourceRange.baseArrayLayer	= 0;
		viewInfo.subresourceRange.layerCount		= 1;

		if (vkCreateImageView(*m_logicalDevice, &viewInfo, nullptr, &m_imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}
	}

	void Image::transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = m_logicalDevice->beginSingleTimeCommand();

		VkImageMemoryBarrier barrier{};
		barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout						= oldLayout;
		barrier.newLayout						= newLayout;
		barrier.srcQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
		barrier.image							= m_image;
		barrier.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel	= 0;
		barrier.subresourceRange.levelCount		= 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount		= 1;
		barrier.srcAccessMask					= 0;
		barrier.dstAccessMask					= 0;

		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags dstStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::runtime_error("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			srcStage, dstStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		m_logicalDevice->endSingleTimeCommand(commandBuffer);
	}

	void Image::copyFromBuffer(VkBuffer buffer, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = m_logicalDevice->beginSingleTimeCommand();

		VkBufferImageCopy region{};
		region.bufferOffset			= 0;
		region.bufferRowLength		= 0;
		region.bufferImageHeight	= 0;

		region.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel		= 0;
		region.imageSubresource.baseArrayLayer	= 0;
		region.imageSubresource.layerCount		= 1;

		region.imageOffset = { 0,0,0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			m_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region);

		m_logicalDevice->endSingleTimeCommand(commandBuffer);
	}
}
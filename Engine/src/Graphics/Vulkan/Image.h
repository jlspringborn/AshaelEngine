/**
 * Wrapper class for Vulkan image and image view
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Vulkan\LogicalDevice.h"
#include "Vulkan\PhysicalDevice.h"

#include <vulkan/vulkan.h>

namespace ash
{
	/**
	 * Wrapper class for Vulkan image and image view
	 */
	class Image
	{
	public:
		Image(
			const LogicalDevice* logicalDevice,
			const PhysicalDevice* physicalDevice,
			uint32_t width,
			uint32_t height,
			VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkImageAspectFlags aspectFlags
		);
		~Image();

		/**
		 * overide * operator for more intuitive access
		 * returns image view
		 */
		operator const VkImageView& () const { return m_imageView; }

		/**
		 * Creates the Vulkan Image
		 */
		void createImage(
			const PhysicalDevice* physicalDevice, 
			uint32_t width, 
			uint32_t height, 
			VkFormat format, 
			VkImageTiling tiling, 
			VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties
		);

		/**
		 * Creates the Vulkan Image View used to access the Vulkan Image
		 */
		void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);

		/**
		 * Transitions image from one format to another using Vulkan Barriers
		 */
		void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		/**
		 * Copy image data from provided buffer into the Vulkan Image
		 */
		void copyFromBuffer(VkBuffer buffer, uint32_t width, uint32_t height);

	private:

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};

		/**
		 * Vulkan Image, loaded from image files such as png and jpg
		 */
		VkImage m_image{};

		/**
		 * Vulkan Device Memory, memory of stored Vulkan Image
		 */
		VkDeviceMemory m_imageMemory{};

		/**
		 * Vulkan Image View, used to access data in the Vulkan Image
		 */
		VkImageView m_imageView{};
	};
}
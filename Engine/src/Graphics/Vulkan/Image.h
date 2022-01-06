#pragma once

#include "Vulkan\LogicalDevice.h"
#include "Vulkan\PhysicalDevice.h"

#include <vulkan/vulkan.h>

namespace ash
{
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

		void createImage(
			const PhysicalDevice* physicalDevice, 
			uint32_t width, 
			uint32_t height, 
			VkFormat format, 
			VkImageTiling tiling, 
			VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties
		);


		void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);

		void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		void copyFromBuffer(VkBuffer buffer, uint32_t width, uint32_t height);

	private:

		const LogicalDevice* m_logicalDevice{};

		VkImage m_image{};

		VkDeviceMemory m_imageMemory{};

		VkImageView m_imageView{};
	};
}
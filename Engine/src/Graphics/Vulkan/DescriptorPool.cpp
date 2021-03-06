/**
 * Copyright (C) 2021, Jesse Springborn
 */
#include "Vulkan/DescriptorPool.h"

#include <stdexcept>
#include <array>

namespace ash
{
	DescriptorPool::DescriptorPool(const LogicalDevice* logicalDevice, const uint32_t swapChainImagecount) :
		m_logicalDevice{ logicalDevice }
	{
		createDescriptorPool(swapChainImagecount);
	}

	DescriptorPool::~DescriptorPool()
	{
		cleanupDescriptorPool();
	}

	// TODO: figure out way to calculate desired descriptor count
	void DescriptorPool::createDescriptorPool(const uint32_t swapChainImagecount)
	{
		// all descriptors used by the shader
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount	= 100;	
		poolSizes[1].type				= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount	= 100;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount	= static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes		= poolSizes.data();
		poolInfo.maxSets		= 100;

		if (vkCreateDescriptorPool(*m_logicalDevice, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void DescriptorPool::cleanupDescriptorPool()
	{
		vkDestroyDescriptorPool(*m_logicalDevice, m_descriptorPool, nullptr);
	}
}
#include "Vulkan/DescriptorPool.h"

#include <stdexcept>

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

	void DescriptorPool::createDescriptorPool(const uint32_t swapChainImagecount)
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = swapChainImagecount;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = swapChainImagecount;

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
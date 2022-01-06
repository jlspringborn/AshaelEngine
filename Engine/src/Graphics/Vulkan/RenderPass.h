#pragma once

#include "Vulkan/LogicalDevice.h"
#include "Vulkan/SwapChain.h"

#include <vulkan/vulkan.h>

namespace ash
{
	class RenderPass
	{
	public:
		RenderPass(const LogicalDevice* logicalDevice, const SwapChain* swapChain, const PhysicalDevice* physicalDevice);
		~RenderPass();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkRenderPass& () const { return m_renderPass; }

		void cleanupRenderPass();

		void createRenderPass(const SwapChain* swapChain, const PhysicalDevice* physicalDevice);

	private:


		VkRenderPass m_renderPass{};

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};
		

	};
}
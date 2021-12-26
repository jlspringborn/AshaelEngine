#pragma once

#include "Vulkan/LogicalDevice.h"
#include "Vulkan/SwapChain.h"

#include <vulkan/vulkan.h>

namespace ash
{
	class RenderPass
	{
	public:
		RenderPass(const LogicalDevice* logicalDevice, const SwapChain* swapChain);
		~RenderPass();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkRenderPass& () const { return m_renderPass; }

		void cleanupRenderPass();

		void createRenderPass(const SwapChain* swapChain);

	private:

		VkRenderPass m_renderPass{};

		const LogicalDevice* m_logicalDevice{};
		

	};
}
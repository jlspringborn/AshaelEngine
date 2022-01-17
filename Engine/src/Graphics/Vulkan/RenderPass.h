/**
* Wrapper for Vulkan RenderPass
*
* Copyright (C) 2021, Jesse Springborn
*/
#pragma once

#include "Vulkan/LogicalDevice.h"
#include "Vulkan/SwapChain.h"

#include <vulkan/vulkan.h>

namespace ash
{
	/**
	 * Wrapper for the Vulkan RenderPass
	 */
	class RenderPass
	{
	public:
		RenderPass(const LogicalDevice* logicalDevice, const SwapChain* swapChain, const PhysicalDevice* physicalDevice);
		~RenderPass();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkRenderPass& () const { return m_renderPass; }

		/**
		 * Cleans up Vulkan RenderPass, called during swap chain recreation
		 */
		void cleanupRenderPass();

		/**
		 * Creates the Vulkan RenderPass
		 */
		void createRenderPass(const SwapChain* swapChain, const PhysicalDevice* physicalDevice);

	private:
		
		/**
		 * Vulkan RenderPass, contains configuration info for upcoming render operations
		 */
		VkRenderPass m_renderPass{};

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};
		

	};
}
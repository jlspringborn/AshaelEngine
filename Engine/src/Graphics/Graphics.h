/**
 * Manages app graphical interface
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Window.h"
#include "Vulkan\Instance.h"
#include "Vulkan\DebugMessenger.h"
#include "Vulkan\Surface.h"
#include "Vulkan\PhysicalDevice.h"
#include "Vulkan\LogicalDevice.h"
#include "Vulkan\SwapChain.h"
#include "Vulkan\RenderPass.h"
#include "Vulkan\GraphicsPipeline.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

namespace ash
{
	class Graphics
	{
	public:
		Graphics(Window* window);
		~Graphics();

		void renderGameObjects();

		void waitForDeviceIdle();

	private:

		Window* m_window{};

		std::unique_ptr<Instance> m_instance{};
		std::unique_ptr<DebugMessenger> m_debugMessenger{};
		std::unique_ptr<Surface> m_surface{};
		std::unique_ptr<PhysicalDevice> m_physicalDevice{};
		std::unique_ptr<LogicalDevice> m_logicalDevice{};
		std::unique_ptr<SwapChain> m_swapChain{};
		std::unique_ptr<RenderPass> m_renderPass{};
		std::unique_ptr<GraphicsPipeline> m_graphicsPipeline{};
		
		std::vector<VkCommandBuffer> m_commandBuffers{};

		std::vector<VkSemaphore> m_imageAvailableSemaphores{};
		std::vector<VkSemaphore> m_renderFinishedSemaphores{};
		std::vector<VkFence> m_inFlightFences{};
		std::vector<VkFence> m_imagesInFlight{};

		const int m_maxFramesInFlight{ 2 };

		size_t m_currentFrame = 0;

		void createCommandBuffers();

		void createSyncObjects();

		void startRenderPass(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer);

		void endRenderPass(VkCommandBuffer commandBuffer);

		void cleanupSwapChain();

		void recreateSwapChain();

		void cleanupCommandBuffers();

		void cleanupSyncObjects();

	};
}
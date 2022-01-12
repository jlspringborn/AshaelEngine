/**
 * Manages the apps graphical interface
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
#include "Vulkan\DescriptorPool.h"
#include "Vulkan\Image.h"
#include "Model.h"

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

		void renderGameObjects(std::vector<std::unique_ptr<Model>>& gameObjects);

		void waitForDeviceIdle();


		Window* m_window{};

		std::unique_ptr<Instance> m_instance{};
		std::unique_ptr<DebugMessenger> m_debugMessenger{};
		std::unique_ptr<Surface> m_surface{};
		std::unique_ptr<PhysicalDevice> m_physicalDevice{};
		std::unique_ptr<LogicalDevice> m_logicalDevice{};
		std::unique_ptr<SwapChain> m_swapChain{};
		std::unique_ptr<RenderPass> m_renderPass{};
		std::unique_ptr<GraphicsPipeline> m_graphicsPipeline{};
		std::unique_ptr<DescriptorPool> m_descriptorPool{};
		std::unique_ptr<Image> m_depthImage{};
		

		std::vector<VkCommandBuffer> m_commandBuffers{};

		std::vector<VkSemaphore> m_imageAvailableSemaphores{};
		std::vector<VkSemaphore> m_renderFinishedSemaphores{};
		std::vector<VkFence> m_inFlightFences{};
		std::vector<VkFence> m_imagesInFlight{};

		const int m_maxFramesInFlight{ 2 };

		size_t m_currentFrame = 0;

		VkDescriptorSetLayout m_descriptorSetLayout{};

		VkSampler m_textureSampler{};

		/**
		 * Array of Uniform buffers, one for each swap chain image
		 */
		std::vector<std::unique_ptr<Buffer>> m_uniformBuffers;
	private:

		void createCommandBuffers();

		void createSyncObjects();

		void startRenderPass(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer);

		void endRenderPass(VkCommandBuffer commandBuffer);

		void cleanupSwapChain(std::vector<std::unique_ptr<Model>>& gameObjects);

		void recreateSwapChain(std::vector<std::unique_ptr<Model>>& gameObjects);

		void cleanupCommandBuffers();

		void cleanupSyncObjects();

		void updateUniformBuffers(uint32_t currentImage, std::vector<std::unique_ptr<Model>>& gameObjects);

		void createDescriptorSetLayout();

		void cleanupDescriptorSetLayout();

		void createTextureSampler();

		void cleanupTextureSampler();

		void createDepthResources();

		void cleanupDepthResource();

		bool hasStencilComponent(VkFormat format);

		/**
		 * Creates a uniform buffer for each swap chain frame to allow for
		 * matrix transformations during shading
		 */
		void createUniformBuffers();

		/**
		 * Deletes all uniform buffers, called during swap chain recreation
		 */
		void cleanupUniformBuffers();

		/**
		 * Updates uniform buffers with new transform data
		 */
		void updateUniformBuffer(uint32_t currentImage, VkExtent2D extent);




	};
}
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
#include "GameObjects/GameObject.h"
#include "Camera/Camera.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>
#include <string>

namespace ash
{
	class Graphics
	{
	public:
		Graphics(Window* window);
		~Graphics();

		/**
		 * Renders array of provided GameObjects based on provided Camera info
		 */
		void renderGameObjects(std::vector<std::unique_ptr<GameObject>>& gameObjects,Camera* camera);

		/**
		 * Waits for the Vulkan device to finish processing
		 */
		void waitForDeviceIdle();

		/**
		 * Generates a Model, this is here because Model.h requires access to the Vulkan device
		 */
		std::unique_ptr<Model> generateModel(std::string modelPath, std::string texturePath);

		/**
		 * Gets the current aspect ration of the swap chain images
		 */
		float getAspectRatio() const { return m_swapChain->getAspectRatio(); }

	private:

		/**
		 * Window
		 */
		Window* m_window{};

		/**
		 * Vulkan Instance Wrapper, used to access Vulkan Library
		 */
		std::unique_ptr<Instance> m_instance{};

		/**
		 * Vulkan Debug Messenger Wrapper, used to display debug messages to the
		 * screen during debug mode
		 */
		std::unique_ptr<DebugMessenger> m_debugMessenger{};

		/**
		 * Vulkan Surface Wrapper, used to write image data to the screen
		 */
		std::unique_ptr<Surface> m_surface{};

		/**
		 * Vulkan Physical Device Wrapper, used to access the GPU
		 */
		std::unique_ptr<PhysicalDevice> m_physicalDevice{};

		/**
		 * Vulkan Logical Device Wrapper, logical interface for the physical GPU
		 */
		std::unique_ptr<LogicalDevice> m_logicalDevice{};

		/**
		 * Vulkan Swap Chain Wrapper, used to manage which images are being written
		 * to and which images are being presented to the screen
		 */
		std::unique_ptr<SwapChain> m_swapChain{};

		/**
		 * Vulkan RenderPass Wrapper, contains configuration data for upcoming 
		 * rendering operations, links into pipeline
		 */
		std::unique_ptr<RenderPass> m_renderPass{};

		/**
		 * Vulkan Graphics Pipeline Wrapper, contains all configuration data for
		 * render operations
		 */
		std::unique_ptr<GraphicsPipeline> m_graphicsPipeline{};

		/**
		 * Vulkan Descriptor Pool Wrapper, manages allocation of descriptor sets
		 */
		std::unique_ptr<DescriptorPool> m_descriptorPool{};

		/**
		 * Used for determining draw order of objects
		 */
		std::unique_ptr<Image> m_depthImage{};
		
		/**
		 * Array of Vulkan Command Buffers, used for recording render operations
		 */
		std::vector<VkCommandBuffer> m_commandBuffers{};

		/**
		 * Vulkan Semaphores, waits to acquire a swap chain image until an image is available
		 * GPU - GPU sync
		 */
		std::vector<VkSemaphore> m_imageAvailableSemaphores{};

		/**
		 * Vulkan Semaphores, signals that a render is finished
		 * GPU - GPU sync
		 */
		std::vector<VkSemaphore> m_renderFinishedSemaphores{};

		/**
		 * Vulkan Fences, wait to submit draw call until frame is ready
		 * GPU - CPU sync
		 */
		std::vector<VkFence> m_inFlightFences{};

		/**
		 * Vulkan Fences, wait for images to finish operations before using them
		 * GPU - CPU sync
		 */
		std::vector<VkFence> m_imagesInFlight{};

		/**
		 * The maximum number of images that can be in flight at any given time
		 */
		const int m_maxFramesInFlight{ 2 };

		/**
		 * Used for sync operations
		 */
		size_t m_currentFrame = 0;

		/**
		 * Vulkan Descriptor Set Layout, used to tell shaders what kind of data
		 * to expect from uniform buffers & push constants
		 */
		VkDescriptorSetLayout m_descriptorSetLayout{};

		/**
		 * Vulkan Sampler, samples image textures for displaying textures to models
		 */
		VkSampler m_textureSampler{};

		/**
		 * Array of Uniform buffers, one for each swap chain image
		 */
		std::vector<std::unique_ptr<Buffer>> m_uniformBuffers;

		/**
		 * Creates Vulkan Command buffers, called during swap chain recreation
		 */
		void createCommandBuffers();

		/**
		 * Create semaphores and fences
		 * Calling during swap chain recreation will produce error
		 * TODO: look into this
		 */
		void createSyncObjects();

		/**
		 * Initiates the Vulkan Render Pass, start render pass for binding and 
		 * draw call recording
		 */
		void startRenderPass(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer);

		/**
		 * Ends the Vulkan Render Pass, stops recording of binding and draw calls
		 * for the current frame
		 */
		void endRenderPass(VkCommandBuffer commandBuffer);

		/**
		 * Cleans up swap chain, gameObjects are needed since descriptor sets are
		 * linked to the swap chain image count
		 */
		void cleanupSwapChain(std::vector<std::unique_ptr<GameObject>>& gameObjects);

		/**
		 * Recreates Vulkan Swap Chain, gameObjects are needed since descriptor sets are
		 * linked to the swap chain image count
		 */
		void recreateSwapChain(std::vector<std::unique_ptr<GameObject>>& gameObjects);

		/**
		 * Called during swap chain recreation and when class is destroyed
		 */
		void cleanupCommandBuffers();

		/**
		 * Called in destructor
		 */
		void cleanupSyncObjects();

		/**
		 * Called in constructor
		 */
		void createDescriptorSetLayout();

		/**
		 * Called in destructor
		 */
		void cleanupDescriptorSetLayout();

		/**
		 * Called in constructor
		 */
		void createTextureSampler();

		/**
		 * Called in destructor
		 */
		void cleanupTextureSampler();

		/**
		 * Called in constructor
		 */
		void createDepthResources();

		/**
		 * Called in destructor
		 */
		void cleanupDepthResource();

		/**
		 * True if provided format is compatible with a stencil component
		 */
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
		void updateUniformBuffer(uint32_t currentImage, VkExtent2D extent,Camera* camera);
	};
}
#include "Graphics.h"

#include "Vulkan/UniformBufferObject.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <array>

namespace ash
{
	/**
	 * initializer list is not used for debugger creation which must happen 
	 * after the instance is created but before all other components
	 */
	Graphics::Graphics(Window* window) :
		m_window{ window }
	{
		m_instance = std::make_unique<Instance>();
		if (m_instance->isValidationEnabled)
		{
			m_debugMessenger = std::make_unique<DebugMessenger>(m_instance.get());
		}
		m_surface			= std::make_unique<Surface>(m_instance.get(), m_window);
		m_physicalDevice	= std::make_unique<PhysicalDevice>(m_instance.get(), m_surface.get());
		m_logicalDevice		= std::make_unique<LogicalDevice>(m_instance.get(), m_physicalDevice.get());
		m_swapChain			= std::make_unique<SwapChain>(m_window, m_surface.get(), m_physicalDevice.get(), m_logicalDevice.get());
		m_descriptorPool	= std::make_unique<DescriptorPool>(m_logicalDevice.get(), m_swapChain->getImageCount());
		createDescriptorSetLayout();
		createTextureSampler();

		m_renderPass		= std::make_unique<RenderPass>(m_logicalDevice.get(), m_swapChain.get(), m_physicalDevice.get());
		m_graphicsPipeline	= std::make_unique<GraphicsPipeline>(m_logicalDevice.get(), m_swapChain.get(), m_renderPass.get(), m_descriptorSetLayout);

		createDepthResources();
		// must be called after render pass creation
		m_swapChain->createFramebuffers(*m_renderPass, *m_depthImage);


		createCommandBuffers();
		createSyncObjects();
		createUniformBuffers();
	}

	Graphics::~Graphics()
	{
		cleanupSyncObjects();
		cleanupCommandBuffers();
		cleanupDescriptorSetLayout();
		cleanupTextureSampler();
	}

	void Graphics::renderGameObjects(std::vector<std::unique_ptr<GameObject>>& gameObjects,Camera* camera)
	{
		vkWaitForFences(*m_logicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		
		VkResult result = vkAcquireNextImageKHR(*m_logicalDevice, *m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain(gameObjects);
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE)
		{
			vkWaitForFences(*m_logicalDevice, 1, &m_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}

		m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		startRenderPass(m_swapChain->getFramebuffers()[imageIndex], m_commandBuffers[imageIndex]);
		for (size_t i = 0; i < gameObjects.size(); i++)
		{
			gameObjects[i]->draw(m_commandBuffers[imageIndex], m_graphicsPipeline->getLayout(), imageIndex);
		}
		endRenderPass(m_commandBuffers[imageIndex]);

		//
	
		VkSemaphore waitSemaphores[]		= { m_imageAvailableSemaphores[m_currentFrame] };
		VkPipelineStageFlags waitStages[]	= { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[]		= { m_renderFinishedSemaphores[m_currentFrame] };

		updateUniformBuffer(imageIndex, m_swapChain->getSwapExtent(), camera);

		VkSubmitInfo submitInfo{};
		submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount	= 1;
		submitInfo.pWaitSemaphores		= waitSemaphores;
		submitInfo.pWaitDstStageMask	= waitStages;
		submitInfo.commandBufferCount	= 1;
		submitInfo.pCommandBuffers		= &m_commandBuffers[imageIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores	= signalSemaphores;

		vkResetFences(*m_logicalDevice, 1, &m_inFlightFences[m_currentFrame]);

		if (vkQueueSubmit(m_logicalDevice->getGraphicQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command to buffer!");
		}

		VkSwapchainKHR swapChains[] = { *m_swapChain };
		
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount	= 1;
		presentInfo.pWaitSemaphores		= signalSemaphores;
		presentInfo.swapchainCount		= 1;
		presentInfo.pSwapchains			= swapChains;
		presentInfo.pImageIndices		= &imageIndex;
		presentInfo.pResults			= nullptr;

		result = vkQueuePresentKHR(m_logicalDevice->getPresentQueue(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window->getWasWindowResized())
		{
			m_window->resetWasWindowResized();
			recreateSwapChain(gameObjects);
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
	}

	void Graphics::waitForDeviceIdle()
	{
		vkDeviceWaitIdle(*m_logicalDevice);
	}

	std::unique_ptr<Model> Graphics::generateModel(std::string modelPath, std::string texturePath)
	{
		std::unique_ptr<Model> model = std::make_unique<Model>
			(
			m_logicalDevice.get(), 
			m_physicalDevice.get(),
			m_swapChain->getImageCount(), 
			m_descriptorSetLayout, 
			m_descriptorPool->getPool(), 
			m_textureSampler,
			m_uniformBuffers,
			modelPath,
			texturePath
			);
		return std::move(model);
	}


	void Graphics::createCommandBuffers()
	{
		m_commandBuffers.resize(m_swapChain->getFramebuffers().size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool			= m_logicalDevice->getCommandPool();
		allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount	= (uint32_t)m_commandBuffers.size();

		if (vkAllocateCommandBuffers(*m_logicalDevice, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void Graphics::createSyncObjects()
	{
		m_imageAvailableSemaphores.resize(m_maxFramesInFlight);
		m_renderFinishedSemaphores.resize(m_maxFramesInFlight);
		m_inFlightFences.resize(m_maxFramesInFlight);
		m_imagesInFlight.resize(m_swapChain->getImageCount(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < m_maxFramesInFlight; ++i)
		{
			if ((vkCreateSemaphore(*m_logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS)
				|| (vkCreateSemaphore(*m_logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS)
				|| (vkCreateFence(*m_logicalDevice, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS))
			{
				throw std::runtime_error("failed to create sync objects!");
			}
		}

	}

	void Graphics::startRenderPass(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer)
	{
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].color = { 1.0f, 0 };
		
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass			= *m_renderPass;
		renderPassInfo.framebuffer			= framebuffer;
		renderPassInfo.renderArea.offset	= { 0,0 };
		renderPassInfo.renderArea.extent	= m_swapChain->getSwapExtent();
		renderPassInfo.clearValueCount		= static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues			= clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_graphicsPipeline);
		
	}

	void Graphics::endRenderPass(VkCommandBuffer commandBuffer)
	{
		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void Graphics::recreateSwapChain(std::vector<std::unique_ptr<GameObject>>& gameObjects)
	{
		int width{ 0 };
		int height{ 0 };
		glfwGetFramebufferSize(m_window->getWindow(), &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(m_window->getWindow(), &width, &height);
			glfwWaitEvents();
		}

		waitForDeviceIdle();
		
		cleanupSwapChain(gameObjects);

		m_swapChain			->createSwapChain(m_window, m_surface.get(), m_physicalDevice.get());
		m_swapChain			->createImageViews();
		m_renderPass		->createRenderPass(m_swapChain.get(), m_physicalDevice.get());
		m_graphicsPipeline	->createPipeline(m_swapChain.get(), m_renderPass.get(), m_descriptorSetLayout);
		createDepthResources();
		m_swapChain			->createFramebuffers(*m_renderPass, *m_depthImage);

		m_descriptorPool->createDescriptorPool(m_swapChain->getImageCount());
		createCommandBuffers();

		createUniformBuffers();

		for (size_t i = 0; i < gameObjects.size(); i++)
		{
			gameObjects[i]->m_model->createDescriptorSets(m_swapChain->getImageCount(), m_descriptorSetLayout, *m_descriptorPool, m_textureSampler, m_uniformBuffers);
		}
	}

	void Graphics::cleanupCommandBuffers()
	{
		vkFreeCommandBuffers(*m_logicalDevice, m_logicalDevice->getCommandPool(),
			static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
	}

	void Graphics::cleanupSyncObjects()
	{
		for (size_t i = 0; i < m_maxFramesInFlight; i++)
		{
			vkDestroySemaphore(*m_logicalDevice, m_renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(*m_logicalDevice, m_imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(*m_logicalDevice, m_inFlightFences[i], nullptr);
		}
	}

	void Graphics::createDescriptorSetLayout()
	{
		// uniform buffer
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding			= 0;
		uboLayoutBinding.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount	= 1;
		uboLayoutBinding.stageFlags			= VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		// image sampler
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding			= 1;
		samplerLayoutBinding.descriptorCount	= 1;
		samplerLayoutBinding.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags			= VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(*m_logicalDevice, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void Graphics::cleanupDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(*m_logicalDevice, m_descriptorSetLayout, nullptr);
	}

	void Graphics::createTextureSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;	// over sampling
		samplerInfo.minFilter = VK_FILTER_LINEAR;	// under sampling
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = m_physicalDevice->getProperties().limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(*m_logicalDevice, &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	void Graphics::cleanupTextureSampler()
	{
		vkDestroySampler(*m_logicalDevice, m_textureSampler, nullptr);
	}

	void Graphics::createDepthResources()
	{
		VkFormat depthFormat = m_physicalDevice->findDepthFormat();

		VkExtent2D extent{ m_swapChain->getSwapExtent() };

		m_depthImage = std::make_unique<Image>(
			m_logicalDevice.get(),
			m_physicalDevice.get(),
			extent.width,
			extent.height,
			depthFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void Graphics::cleanupDepthResource()
	{
		m_depthImage = nullptr;
	}

	bool Graphics::hasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void Graphics::createUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		m_uniformBuffers.resize(m_swapChain->getImageCount());

		for (size_t i = 0; i < m_swapChain->getImageCount(); i++)
		{
			m_uniformBuffers[i] = std::make_unique<Buffer>(m_logicalDevice.get(),
				m_physicalDevice.get(),
				bufferSize,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
	}

	void Graphics::cleanupUniformBuffers()
	{
		size_t bufferCount{ m_uniformBuffers.size() };
		for (size_t i = 0; i < bufferCount; i++)
		{
			m_uniformBuffers[i] = nullptr;
		}
	}

	void Graphics::updateUniformBuffer(uint32_t currentImage, VkExtent2D extent, Camera* camera)
	{
		UniformBufferObject ubo{};
		//ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.f);
		//ubo.proj[1][1] *= -1;

		ubo.proj = camera->getProjection();
		ubo.view = camera->getView();

		void* data;
		vkMapMemory(*m_logicalDevice, m_uniformBuffers[currentImage]->getBufferMemory(), 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(*m_logicalDevice, m_uniformBuffers[currentImage]->getBufferMemory());
	}

	void Graphics::cleanupSwapChain(std::vector<std::unique_ptr<GameObject>>& gameObjects)
	{
		for (size_t i = 0; i < gameObjects.size(); i++)
		{
			gameObjects[i]->m_model->cleanupDescriptorSets();
		}

		cleanupUniformBuffers();
		cleanupCommandBuffers();
		cleanupDepthResource();
		m_swapChain			->cleanupFramebuffers();
		m_graphicsPipeline	->cleanupPipeline();
		m_renderPass		->cleanupRenderPass();
		m_swapChain			->cleanupImageViews();
		m_swapChain			->cleanupSwapChain();
		m_descriptorPool	->cleanupDescriptorPool();

	}
}
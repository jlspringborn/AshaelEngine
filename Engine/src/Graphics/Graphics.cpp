#include "Graphics.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

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
		m_model				= std::make_unique<Model>(m_logicalDevice.get(), m_physicalDevice.get(), m_swapChain->getImageCount(), m_descriptorSetLayout, *m_descriptorPool);
		m_renderPass		= std::make_unique<RenderPass>(m_logicalDevice.get(), m_swapChain.get());
		m_graphicsPipeline	= std::make_unique<GraphicsPipeline>(m_logicalDevice.get(), m_swapChain.get(), m_renderPass.get(), m_descriptorSetLayout);

		// must be called after render pass creation
		m_swapChain->createFramebuffers(*m_renderPass);


		createCommandBuffers();
		createSyncObjects();


		//////////////////////////////////////////////////////////////////////////
		// TODO: move command buffer recording to renderGameObjects and record
		// every frame instead

		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags				= 0;
			beginInfo.pInheritanceInfo	= nullptr;

			if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			startRenderPass(m_swapChain->getFramebuffers()[i], m_commandBuffers[i]);
			m_model->draw(m_commandBuffers[i], m_graphicsPipeline->getLayout(), i);
			endRenderPass(m_commandBuffers[i]);
		}
		//////////////////////////////////////////////////////////////////////////
	}

	Graphics::~Graphics()
	{
		cleanupSyncObjects();
		cleanupCommandBuffers();
		cleanupDescriptorSetLayout();
	}

	void Graphics::renderGameObjects()
	{
		vkWaitForFences(*m_logicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		
		VkResult result = vkAcquireNextImageKHR(*m_logicalDevice, *m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
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

		
		VkSemaphore waitSemaphores[]		= { m_imageAvailableSemaphores[m_currentFrame] };
		VkPipelineStageFlags waitStages[]	= { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[]		= { m_renderFinishedSemaphores[m_currentFrame] };

		updateUniformBuffers(imageIndex);

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
			recreateSwapChain();
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
		VkClearValue clearColor	= { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass			= *m_renderPass;
		renderPassInfo.framebuffer			= framebuffer;
		renderPassInfo.renderArea.offset	= { 0,0 };
		renderPassInfo.renderArea.extent	= m_swapChain->getSwapExtent();
		renderPassInfo.clearValueCount		= 1;
		renderPassInfo.pClearValues			= &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// TODO: move this to the model class later
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

	void Graphics::recreateSwapChain()
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
		
		cleanupSwapChain();

		m_swapChain			->createSwapChain(m_window, m_surface.get(), m_physicalDevice.get());
		m_swapChain			->createImageViews();
		m_renderPass		->createRenderPass(m_swapChain.get());
		m_graphicsPipeline	->createPipeline(m_swapChain.get(), m_renderPass.get(), m_descriptorSetLayout);
		m_swapChain			->createFramebuffers(*m_renderPass);

		m_model->createUniformBuffers(m_physicalDevice.get(), m_swapChain->getImageCount());
		m_descriptorPool->createDescriptorPool(m_swapChain->getImageCount());
		m_model->createDescriptorSets(m_swapChain->getImageCount(), m_descriptorSetLayout, *m_descriptorPool);
		createCommandBuffers();

		//////////////////////////////////////////////////////////////////////////
		// TODO: move command buffer recording to renderGameObjects and record
		// every frame instead
		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags				= 0;
			beginInfo.pInheritanceInfo	= nullptr;

			if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			startRenderPass(m_swapChain->getFramebuffers()[i], m_commandBuffers[i]);
			m_model->draw(m_commandBuffers[i], m_graphicsPipeline->getLayout(), i);
			endRenderPass(m_commandBuffers[i]);
		}
		//////////////////////////////////////////////////////////////////////////
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

	void Graphics::updateUniformBuffers(uint32_t currentImage)
	{
		m_model->updateUniformBuffer(currentImage, m_swapChain->getSwapExtent());
	}

	void Graphics::createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(*m_logicalDevice, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void Graphics::cleanupDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(*m_logicalDevice, m_descriptorSetLayout, nullptr);
	}

	void Graphics::cleanupSwapChain()
	{
		cleanupCommandBuffers();
		m_swapChain			->cleanupFramebuffers();
		m_graphicsPipeline	->cleanupPipeline();
		m_renderPass		->cleanupRenderPass();
		m_swapChain			->cleanupImageViews();
		m_swapChain			->cleanupSwapChain();
		m_model				->cleanupUniformBuffers();
		m_descriptorPool	->cleanupDescriptorPool();
	}
}
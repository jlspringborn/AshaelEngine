/**
 * Wrapper for Vulkan Graphics Pipeline
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Vulkan/LogicalDevice.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/RenderPass.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
namespace ash
{
	/**
	 * Wrapper for Vulkan Graphics Pipeline
	 */
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const LogicalDevice* logicalDevice, const SwapChain* swapChain, 
			const RenderPass* renderPass, const VkDescriptorSetLayout& layout);
		~GraphicsPipeline();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkPipeline& () const { return m_graphicsPipeline; }

		/**
		 * Deletes pipeline and layout, called during swap chain recreation
		 */
		void cleanupPipeline();

		/**
		 * Creates a Vulkan Graphics Pipeline, called during swap chain recreation
		 */
		void createPipeline(const SwapChain* swapChain, const RenderPass* renderPass,
			const VkDescriptorSetLayout& layout);

		/**
		 * Returns reference to the pipeline layout
		 */
		const VkPipelineLayout& getLayout() const { return m_pipelineLayout; }

	private:

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};

		/**
		 * Vulkan Pipeline Layout, used during pipeline creation and during draw calls
		 */
		VkPipelineLayout m_pipelineLayout{};

		/**
		 * Vulkan Graphics Pipeline, retrieved using *
		 */
		VkPipeline m_graphicsPipeline{};

		/**
		 * read shader code from provided spirV file
		 */
		static std::vector<char> readFile(const std::string& filename);
		
		/**
		 * convert shader code into shader module
		 */
		VkShaderModule createShaderModule(const std::vector<char>& code);
	};
}
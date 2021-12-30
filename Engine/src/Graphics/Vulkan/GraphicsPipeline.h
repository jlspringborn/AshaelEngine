#pragma once

#include "Vulkan/LogicalDevice.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/RenderPass.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
namespace ash
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const LogicalDevice* logicalDevice, const SwapChain* swapChain, const RenderPass* renderPass, const VkDescriptorSetLayout& layout);
		~GraphicsPipeline();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkPipeline& () const { return m_graphicsPipeline; }

		void cleanupPipeline();

		void createPipeline(const SwapChain* swapChain, const RenderPass* renderPass, const VkDescriptorSetLayout& layout);

		const VkPipelineLayout& getLayout() const { return m_pipelineLayout; }

	private:

		const LogicalDevice* m_logicalDevice{};

		VkPipelineLayout m_pipelineLayout{};

		VkPipeline m_graphicsPipeline{};

		/**
		 * read shader code from provided spv file
		 */
		static std::vector<char> readFile(const std::string& filename);
		
		/**
		 * convert shader code into shader module
		 */
		VkShaderModule createShaderModule(const std::vector<char>& code);
	};
}
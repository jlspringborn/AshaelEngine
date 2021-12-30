/**
 * Wrapper for Vulkan Instance
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>

namespace ash
{
	/**
	 * Wrapper for Vulkan Instance
	 */
	class Instance
	{
	public:
		Instance();
		~Instance();

		/**
		 * Enables validation layers when in debug mode
		 */
#ifdef NDEBUG
		const bool isValidationEnabled = false;
#else
		const bool isValidationEnabled = true;
#endif

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkInstance& () const { return m_instance; }

		/**
		 * check if validation layers are available
		 */
		bool checkValidationLayerSupport();
		
		/**
		 * Returns validation layers
		 */
		const std::vector<const char*>& getValidationLayers() const { return m_validationLayers; }

		/**
		 * Get list of required GLFW extensions
		 */
		std::vector<const char*> getRequiredExtensions();

	private:
		/**
		 * Vulkan Instance
		 */
		VkInstance m_instance{};

		/**
		 * Validation layers for debugging
		 */
		std::vector<const char*> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };

		/**
		 * Prints available extensions to console
		 */
		void printAvailableExtensions();
	};
}
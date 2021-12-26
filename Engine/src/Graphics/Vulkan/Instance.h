/**
 * Manages Vulkan Instance
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
	class Instance
	{
	public:
		Instance();
		~Instance();

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
		
		const std::vector<const char*>& getValidationLayers() const { return m_validationLayers; }

		std::vector<const char*> getRequiredExtensions();

	private:
		VkInstance m_instance{};

		std::vector<const char*> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };

		void printAvailableExtensions();
	};
}
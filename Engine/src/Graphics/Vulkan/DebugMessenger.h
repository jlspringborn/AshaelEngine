/**
* manages vulkan debug messages
* 
* Copyright (C) 2021, Jesse Springborn
*/
#pragma once

#include "Vulkan\Instance.h"

#include <vulkan/vulkan.h>


namespace ash
{
	class DebugMessenger
	{
	public:
		DebugMessenger(const Instance* instance);

		~DebugMessenger();

		VkDebugUtilsMessengerEXT getDebugMessenger() const { return m_debugMessenger; }

		static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	private:
		VkDebugUtilsMessengerEXT m_debugMessenger{};

		const Instance* m_instance{};

		/**
		 * prints vulkan debug messages to console
		 */
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		/**
		 * proxy function to create debug messenger
		 */
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		/**
		 * proxy function to destroy debug messenger
		 */
		static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);

	};
}
/**
* Wrapper for Vulkan debug messages
* 
* Copyright (C) 2021, Jesse Springborn
*/
#pragma once

#include "Vulkan\Instance.h"

#include <vulkan/vulkan.h>


namespace ash
{
	/**
	 * Manages Vulkan debug messages
	 */
	class DebugMessenger
	{
	public:
		DebugMessenger(const Instance* instance);

		~DebugMessenger();

		/**
		 * overide * operator for more intuitive access
		 */
		operator const VkDebugUtilsMessengerEXT& () const { return m_debugMessenger; }

		/**
		 * Fills in data for debug messenger create info, called during creation
		 * and during instance creation before the messenger has been created
		 */
		static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	private:

		/**
		 * Vulkan Instance, used during destruction
		 */
		const Instance* m_instance{};

		/**
		 * Vulkan Debug Messenger
		 */
		VkDebugUtilsMessengerEXT m_debugMessenger{};

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
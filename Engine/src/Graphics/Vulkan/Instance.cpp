/**
 * Copyright (C) 2021, Jesse Springborn
 */
#include "Vulkan\Instance.h"

#include "Vulkan\DebugMessenger.h"	// for instance creation debugging

#include <iostream>
#include <stdexcept>
#include <vector>

namespace ash
{
	Instance::Instance()
	{
		if (isValidationEnabled && !checkValidationLayerSupport())
		{
			throw std::runtime_error("validation layers requested, but not available!");
		}

		// optional app info
		VkApplicationInfo appInfo{};
		appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName	= "Ashael App";
		appInfo.applicationVersion	= VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName			= "Ashael Engine";
		appInfo.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion			= VK_API_VERSION_1_0;

		// instance creation info
		VkInstanceCreateInfo createInfo{};
		createInfo.sType			= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// required extensions for glfw
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount	= static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames	= extensions.data();

		// debug messenger for instance creation
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		// validation layers
		if (isValidationEnabled)
		{
			createInfo.enabledLayerCount	= static_cast<uint32_t>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames	= m_validationLayers.data();

			DebugMessenger::populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount	= 0;
			createInfo.pNext				= nullptr;
		}

		// creation call
		if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vulkan instance!");
		}

		// optional print of extension names
		printAvailableExtensions();
	}

	Instance::~Instance()
	{
		vkDestroyInstance(m_instance, nullptr);
	}

	bool Instance::checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : m_validationLayers)
		{
			bool wasLayerFound{ false };

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)	// true if names are equal
				{
					wasLayerFound = true;
					break;
				}
			}

			if (!wasLayerFound)
			{
				return false;
			}

		}

		return true;
	}

	std::vector<const char*> Instance::getRequiredExtensions()
	{
		uint32_t glfwExtensionCount{ 0 };
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (isValidationEnabled)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void Instance::printAvailableExtensions()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::cout << "available extensions:\n";

		for (const auto& extension : extensions)
		{
			std::cout << '\t' << extension.extensionName << '\n';
		}
	}
}
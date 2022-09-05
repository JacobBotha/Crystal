#pragma once

#include "vulkan/vulkan.h"

namespace Crystal {
	class CRYSTAL_API VulkanInstance {
	public:
		VulkanInstance();
		~VulkanInstance();

		bool IsExtensionAvailable(const char* layer);
		bool IsLayerAvailable(const char* extension);

		VkInstance& GetInstance() { return m_VkInstance; }
		
	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);

		static const bool s_enableValidationLayers;
		static const std::vector<const char*> s_validationLayers;
		
	private:
		void populateDebugMessengerCreateInfo(
			VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void setupDebugMessenger();
		bool checkValidationLayerSupport();
		std::vector<const char*> getRequiredExtensions();
	
		VkInstance m_VkInstance;
		std::vector<const char*> m_Extensions;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
	};
}

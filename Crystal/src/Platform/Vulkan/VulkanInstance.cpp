#include "clpch.h"

#include "VulkanInstance.h"
#include <GLFW/glfw3.h>

namespace Crystal {
#ifdef CL_DEBUG
    const bool VulkanInstance::s_enableValidationLayers = true;
#else
    const bool VulkanInstance::s_enableValidationLayers = false;
#endif // CL_DEBUG

    VkResult VulkanInstance::CreateDebugUtilsMessengerEXT(VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, 
        VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, 
            "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void VulkanInstance::DestroyDebugUtilsMessengerEXT(VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger, 
        const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, 
            "vkDestroyDebugUtilsMessengerEXT");

        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    //Declare the required vulkan validation layers
	const std::vector<const char*> VulkanInstance::s_validationLayers = {
	    "VK_LAYER_KHRONOS_validation"
	};

	VulkanInstance::VulkanInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Crystal Engine";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = "Crystal";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        //Future - insert optional layers and extensions
        m_Extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(m_Extensions.size());
        createInfo.ppEnabledExtensionNames = m_Extensions.data();

        //Enable validation layers if applicable
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (s_enableValidationLayers) {
            CL_CORE_ASSERT(checkValidationLayerSupport(), 
                "Validation layers requested but not supported!");
            createInfo.enabledLayerCount = static_cast<uint32_t>(s_validationLayers.size());
            createInfo.ppEnabledLayerNames = s_validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }
        
        //Create the instance
        if (vkCreateInstance(&createInfo, nullptr, &m_VkInstance) != VK_SUCCESS)
            CL_CORE_CRITICAL("Failed to create vulkan instance!");

	}

    VulkanInstance::~VulkanInstance() {
        if (s_enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugMessenger, nullptr);
        }

        vkDestroyInstance(m_VkInstance, nullptr);
    }

    bool VulkanInstance::IsLayerAvailable(const char* layer) {
        //Future check for more than just validation layers
        for (const char* layerName : s_validationLayers) {
            if (strcmp(layerName, layer) == 0) {
                return true;
            }

            return false;
        }
    }

    bool VulkanInstance::IsExtensionAvailable(const char* extension) {
        for (const char* extensionName : m_Extensions) {
            if (strcmp(extensionName, extension) == 0) {
                return true;
            }

            return false;
        }
    }

    void VulkanInstance::setupDebugMessenger() {
        if (!s_enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(m_VkInstance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }
    
    void VulkanInstance::populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT 
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional
    }

    bool VulkanInstance::checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : s_validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    std::vector<const char*> VulkanInstance::getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (s_enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

		//Check if required extention is available
        uint32_t extensionCount = 0;
        VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        CL_CORE_ASSERT(res == VK_SUCCESS, "Cannot get Vulkan instance extentions!");

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
        
		for (const auto extension : extensions) {
            CL_CORE_INFO("Required vulkan extension: {0}.", extension);
            bool foundExtension = false;
            for (const auto& availableExtension : availableExtensions) {
                if (strcmp(availableExtension.extensionName, extension) == 0) {
                    foundExtension = true;
                    break;
               }
            }

			if (!foundExtension) //Error if extionsion cannot be found
			    CL_CORE_CRITICAL("GLFW Vulkan extention is missing: {0}", extension);
		}

        return extensions;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            //CL_CORE_TRACE("{0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            //CL_CORE_INFO("{0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            CL_CORE_WARN("Vulkan Callback: {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            CL_CORE_ERROR("Vulkan Callback: {0}", pCallbackData->pMessage);
            break;
        default:
            CL_CORE_TRACE("Vulkan Callback: {0}", pCallbackData->pMessage);
            break;
        }

        return VK_FALSE;
    }
}
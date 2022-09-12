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

    void VulkanInstance::DestroyDebugUtilsMessengerEXT(
        VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        CL_CORE_INFO("Ready to destroy debug messenger!");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }

        CL_CORE_INFO("Destroyed debug messenger!");
    }

    //Declare the required vulkan validation layers
	const std::vector<const char*> VulkanInstance::s_validationLayers = {
	    "VK_LAYER_KHRONOS_validation"
	};

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
    {
        (void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
        CL_CORE_TRACE("[vulkan] Debug report from ObjectType: {0}\nMessage: {1}\n\n", objectType, pMessage);
        return VK_FALSE;
    }

	VulkanInstance::VulkanInstance() 
        : m_Allocator(NULL), m_DebugMessenger(VK_NULL_HANDLE), m_DebugReport(VK_NULL_HANDLE) {
        uint32_t extensions_count = 0;
        const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
        // Create Vulkan Instance
		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.enabledExtensionCount = extensions_count;
		create_info.ppEnabledExtensionNames = extensions;

        if (s_enableValidationLayers) {
            // Enabling validation layers
            const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
            create_info.enabledLayerCount = 1;
            create_info.ppEnabledLayerNames = layers;

            // Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
            const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (extensions_count + 1));
            memcpy(extensions_ext, extensions, extensions_count * sizeof(const char*));
            extensions_ext[extensions_count] = "VK_EXT_debug_report";
            create_info.enabledExtensionCount = extensions_count + 1;
            create_info.ppEnabledExtensionNames = extensions_ext;

            // Create Vulkan Instance
            VkResult err = vkCreateInstance(&create_info, m_Allocator, &m_VkInstance);

            //check_vk_result(err);
            free(extensions_ext);

            // Get the function pointer (required for any extensions)
            auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugReportCallbackEXT");
            CL_CORE_ASSERT(vkCreateDebugReportCallbackEXT != NULL, "Debug callback is null");

            // Setup the debug report callback
            VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
            debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
            debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
            debug_report_ci.pfnCallback = debug_report;
            debug_report_ci.pUserData = NULL;
            err = vkCreateDebugReportCallbackEXT(m_VkInstance, &debug_report_ci, m_Allocator, &m_DebugReport);
            //check_vk_result(err);
        } 
        else {
            // Create Vulkan Instance without any debug feature
            VkResult err = vkCreateInstance(&create_info, m_Allocator, &m_VkInstance);
            //check_vk_result(err);
            //IM_UNUSED(g_DebugReport);
        }

	}

    VulkanInstance::~VulkanInstance() {
        if (s_enableValidationLayers) {
            // Remove the debug report callback
            auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugReportCallbackEXT");
            vkDestroyDebugReportCallbackEXT(m_VkInstance, m_DebugReport, m_Allocator);
        }

        vkDestroyInstance(m_VkInstance, m_Allocator);
    }

    bool VulkanInstance::IsLayerAvailable(const char* layer) const {
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

        VkResult result = CreateDebugUtilsMessengerEXT(m_VkInstance, &createInfo, nullptr, &m_DebugMessenger);
        CL_CORE_ASSERT(result == VK_SUCCESS, "Failed to set up debug messenger!");
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
            CL_CORE_TRACE("{0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            CL_CORE_INFO("{0}", pCallbackData->pMessage);
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
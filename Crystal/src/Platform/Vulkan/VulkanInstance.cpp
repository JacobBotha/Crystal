#include "clpch.h"

#include "VulkanInstance.h"
#include <GLFW/glfw3.h>

namespace Crystal {
	VulkanInstance::VulkanInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Crystal Engine";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = "Crystal";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.apiVersion = VK_MAKE_API_VERSION(1, 3, 216, 0);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        //Get vulkan extentions through GLFW.
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        CL_CORE_TRACE("{0}", glfwExtensions[0]);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        //Check which extentions have been loaded and log them
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        
        CL_CORE_INFO("Available Vulkan Extentions:\n");
        for (const auto& extension : extensions)
            CL_CORE_INFO("{0}", extension.extensionName);

        //Create the instance
        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
	}
    VulkanInstance::~VulkanInstance() {
        vkDestroyInstance(m_Instance, nullptr);
    }
}
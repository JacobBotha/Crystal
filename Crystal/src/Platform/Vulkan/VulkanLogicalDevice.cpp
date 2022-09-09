#include "clpch.h"

#include "VulkanLogicalDevice.h"

namespace Crystal {
	VulkanLogicalDevice::VulkanLogicalDevice(
		std::unique_ptr<VulkanPhysicalDevice>& physicalDevice, 
		std::unique_ptr<VulkanInstance>& instance) {

		QueueFamilyIndices indices = physicalDevice->GetQueueFamilies();

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		//No Device Features enabled for now
		VkPhysicalDeviceFeatures deviceFeatures{};
		createInfo.pEnabledFeatures = &deviceFeatures;

		//Only required extensions for now are device extensions
		std::vector extensions = physicalDevice->GetDeviceExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		//Current version of vulkan does not require enabled layers for logical 
		//device - here for legacy support
		std::vector<const char*> enabledLayers = instance->GetEnabledLayers();
		createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
		createInfo.ppEnabledLayerNames = enabledLayers.data();
		
		VkResult result = vkCreateDevice(physicalDevice->GetVkPhysicalDevice(), &createInfo, nullptr, &m_Device);
		CL_CORE_ASSERT(result == VK_SUCCESS, "Could not create logical device!");

		vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, indices.PresentFamily.value(), 0, &m_PresentQueue);
	}

	VulkanLogicalDevice::~VulkanLogicalDevice() {
		CL_CORE_INFO("Destroying device.");
		vkDestroyDevice(m_Device, nullptr);
		CL_CORE_INFO("Device is destroyed!");
	}

}
#include "clpch.h"

#include "VulkanLogicalDevice.h"

namespace Crystal {
	VulkanLogicalDevice::VulkanLogicalDevice(
		std::unique_ptr<VulkanPhysicalDevice>& physicalDevice, 
		std::unique_ptr<VulkanInstance>& instance) {

		QueueFamilyIndices indices = physicalDevice->GetQueueFamilies();

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.GraphicsFamily.value();
		queueCreateInfo.queueCount = 1;

		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;

		//No Device Features enabled for now
		VkPhysicalDeviceFeatures deviceFeatures{};
		createInfo.pEnabledFeatures = &deviceFeatures;

		//No device specific extensions required for now
		createInfo.enabledExtensionCount = 0;

		//Current version of vulkan does not require enabled layers for logical 
		//device - here for legacy support
		std::vector<const char*> enabledLayers = instance->GetEnabledLayers();
		createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
		createInfo.ppEnabledLayerNames = enabledLayers.data();
		
		VkResult result = vkCreateDevice(physicalDevice->GetVkPhysicalDevice(), &createInfo, nullptr, &m_Device);
		CL_CORE_ASSERT(result == VK_SUCCESS, "Could not create logical device!");

		vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
	}

	VulkanLogicalDevice::~VulkanLogicalDevice() {
		CL_CORE_INFO("Destroying device.");
		vkDestroyDevice(m_Device, nullptr);
		CL_CORE_INFO("Device is destroyed!");
	}

}
#include "clpch.h"

#include "VulkanPhysicalDevice.h"

namespace Crystal {
	VulkanPhysicalDevice::VulkanPhysicalDevice(const VkInstance& instance) {
		m_Instance = instance;
		m_PhysicalDevice = VK_NULL_HANDLE;

		SelectPhyiscalDevice();
	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice() {
		
	}

	void VulkanPhysicalDevice::SelectPhyiscalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		CL_CORE_ASSERT(deviceCount, "Failed to find GPU with vulkan support!")

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (IsDeviceSuitable(device)) {
				m_PhysicalDevice = device;
				break;
			}
		}

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

		CL_CORE_INFO("Physical Device Selected: {0}", deviceProperties.deviceName);

		if (m_PhysicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	VulkanPhysicalDevice::QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.GraphicsFamily = i;

			if (indices.IsComplete()) break;

			i++;
		}

		return indices;
	}
	
	bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = FindQueueFamilies(device);

		return indices.IsComplete();
	}
}
#include "clpch.h"

#include "VulkanPhysicalDevice.h"

namespace Crystal {
	VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance) {
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

		CL_CORE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "failed to find a suitable GPU!");
	}
	
	bool VulkanPhysicalDevice::CheckPresentSupport(uint32_t queueFamilyIndex, VkSurfaceKHR surface) const {
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, queueFamilyIndex, surface, &presentSupport);
		return presentSupport == VK_TRUE;
	}

	void VulkanPhysicalDevice::AssignQueueFamilyIndicesWithPresent(VkSurfaceKHR surface) {
		m_QueueFamilyIndices = FindQueueFamiliesWithPresent(m_PhysicalDevice, surface);
	}
	
	QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamiliesWithPresent(VkPhysicalDevice device, VkSurfaceKHR surface) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.GraphicsFamily = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport)
				indices.PresentFamily = i;

			if (indices.IsComplete()) break;

			i++;
		}

		return indices;
	}

	QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device) {
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
		m_QueueFamilyIndices = FindQueueFamilies(device);

		return m_QueueFamilyIndices.IsComplete();
	}
}
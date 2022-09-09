#include "clpch.h"

#include "VulkanPhysicalDevice.h"

namespace Crystal {
	const std::vector<const char*> VulkanPhysicalDevice	::s_DeviceExtentions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanInstance* instance, VulkanSurface* surface) {
		m_Instance = instance->GetInstance();
		m_PhysicalDevice = VK_NULL_HANDLE;

		SelectPhyiscalDevice(surface->GetVkSurface());
	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice() {}

	void VulkanPhysicalDevice::SelectPhyiscalDevice(VkSurfaceKHR surface) {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		CL_CORE_ASSERT(deviceCount, "Failed to find GPU with vulkan support!")

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (IsDeviceSuitable(device, surface)) {
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

	QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
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

	bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
		m_QueueFamilyIndices = FindQueueFamilies(device, surface);
 
		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		return m_QueueFamilyIndices.IsComplete() && extensionsSupported;
	}

	bool VulkanPhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(s_DeviceExtentions.begin(), s_DeviceExtentions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
}
#include "clpch.h"

#include "VulkanPhysicalDevice.h"

namespace Crystal {
	const std::vector<const char*> VulkanPhysicalDevice	::s_DeviceExtentions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanInstance* instance) {
		m_Instance = instance;
		m_PhysicalDevice = VK_NULL_HANDLE;

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance->GetVkInstance(), &deviceCount, nullptr);

		CL_CORE_ASSERT(deviceCount, "Failed to find GPU with vulkan support!")

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance->GetVkInstance(), &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (IsDeviceSuitable(device)) {
				m_PhysicalDevice = device;
				CL_CORE_INFO("Found suitable physical device!");
				break;
			}
		}

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

		CL_CORE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "failed to find a suitable GPU!");

		CL_CORE_INFO("Physical Device Selected: {0}", deviceProperties.deviceName);
	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice() {}

	bool VulkanPhysicalDevice::CheckPresentSupport(QueueFamilyIndex queueFamilyIndex, VkSurfaceKHR surface) const {
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, queueFamilyIndex, surface, &presentSupport);
		return presentSupport == VK_TRUE;
	}

	QueueFamilyIndices VulkanPhysicalDevice::CheckDeviceQueueSupport(VkPhysicalDevice device, QueueFlagBits requiredQueues, VkSurfaceKHR surface) {
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		VkQueueFlags requiredVulkanQueues = 0;
		if (requiredQueues[QueueFlags::Graphics]) {
			requiredVulkanQueues |= VK_QUEUE_GRAPHICS_BIT;
		}
		if (requiredQueues[QueueFlags::Compute]) {
			requiredVulkanQueues |= VK_QUEUE_COMPUTE_BIT;
		}
		if (requiredQueues[QueueFlags::Transfer]) {
			requiredVulkanQueues |= VK_QUEUE_TRANSFER_BIT;
		}

		QueueFamilyIndices indices = {};
		indices.fill(-1);
		VkQueueFlags supportedQueues = 0;
		bool needsPresent = requiredQueues[QueueFlags::Present];
		bool presentSupported = false;

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0) {
				supportedQueues |= queueFamily.queueFlags;
			}

			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices[QueueFlags::Graphics] = i;
			}

			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
				indices[QueueFlags::Compute] = i;
			}

			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
				indices[QueueFlags::Transfer] = i;
			}

			if (needsPresent) {
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
				if (queueFamily.queueCount > 0 && presentSupport) {
					presentSupported = true;
					indices[QueueFlags::Present] = i;
				}
			}

			if ((requiredVulkanQueues & supportedQueues) == requiredVulkanQueues && (!needsPresent || presentSupported)) {
				break;
			}

			i++;
		}

		return indices;
	}

	bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device) {
		//m_QueueFamilyIndices = FindQueueFamilies(device);

		//Check that the graphics, compute, and trasfer queues are supported
		const QueueFlagBits requiredQueues = QueueFlagBit::ComputeBit | QueueFlagBit::GraphicsBit | QueueFlagBit::TransferBit;
		QueueFamilyIndices indices = CheckDeviceQueueSupport(device, requiredQueues);
 
		bool queueSupport = true;
		for (unsigned int i = 0; i < requiredQueues.size(); ++i) {
			if (requiredQueues[i]) {
				queueSupport &= (indices[i] >= 0);
			}
		}
 
		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		return queueSupport && extensionsSupported;
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

	SwapChainSupportDetails VulkanPhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const {
		SwapChainSupportDetails details;
		
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
}
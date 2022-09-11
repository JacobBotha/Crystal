#pragma once

#include "VulkanInstance.h"
#include "Crystal/Renderer/QueueFlags.h"

#include <vulkan/vulkan.h>

namespace Crystal {
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class CRYSTAL_API VulkanPhysicalDevice {
	public:
		VulkanPhysicalDevice(VulkanInstance* instance);
		~VulkanPhysicalDevice();

		VulkanInstance* GetInstance() const { return m_Instance; }
		VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }
		const QueueFamilyIndices GetQueueFamilyIndices(VkSurfaceKHR surface, QueueFlagBits requiredQueues) const { 
			return CheckDeviceQueueSupport(m_PhysicalDevice, requiredQueues, surface); 
		}
		const std::vector<const char*> GetDeviceExtensions() const { return s_DeviceExtentions; }
		//const SwapChainSupportDetails& GetSwapChainSupportDetails() const { return m_SwapChainSupportDetails; }

		bool CheckPresentSupport(QueueFamilyIndex queueFamilyIndex, VkSurfaceKHR surface) const;
		
	private:
		static QueueFamilyIndices CheckDeviceQueueSupport(VkPhysicalDevice device, QueueFlagBits requiredQueues, VkSurfaceKHR surface = VK_NULL_HANDLE);

		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const;

		static const std::vector<const char*> s_DeviceExtentions;

		VulkanInstance* m_Instance;

		VkPhysicalDevice m_PhysicalDevice;
		SwapChainSupportDetails m_SwapChainSupportDetails;
	};
}

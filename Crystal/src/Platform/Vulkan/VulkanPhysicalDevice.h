#pragma once

#include "VulkanInstance.h"
#include "Crystal/Renderer/QueueFlags.h"

#include <vulkan/vulkan.h>

namespace Crystal {
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

		bool CheckPresentSupport(QueueFamilyIndex queueFamilyIndex, VkSurfaceKHR surface) const;

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		
	private:
		static QueueFamilyIndices CheckDeviceQueueSupport(VkPhysicalDevice device, QueueFlagBits requiredQueues, VkSurfaceKHR surface = VK_NULL_HANDLE);

		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);


		static const std::vector<const char*> s_DeviceExtentions;

		VulkanInstance* m_Instance;

		VkPhysicalDevice m_PhysicalDevice;
	};
}

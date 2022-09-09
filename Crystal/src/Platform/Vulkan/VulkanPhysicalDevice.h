#pragma once

#include "VulkanInstance.h"
#include "VulkanSurface.h"

namespace Crystal {
	struct QueueFamilyIndices {
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool IsComplete() {
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class CRYSTAL_API VulkanPhysicalDevice {
	public:
		VulkanPhysicalDevice(VulkanInstance* instance, VulkanSurface* surface);
		~VulkanPhysicalDevice();

		inline VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }
		inline QueueFamilyIndices GetQueueFamilies() const { return m_QueueFamilyIndices; }
		bool CheckPresentSupport(uint32_t queueFamilyIndex, VkSurfaceKHR surface) const;

		const std::vector<const char*> GetDeviceExtensions() const { return s_DeviceExtentions; }
		
	private:
		static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
		bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
		void SelectPhyiscalDevice(VkSurfaceKHR surface);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;

		static const std::vector<const char*> s_DeviceExtentions;

		VkPhysicalDevice m_PhysicalDevice;
		VkInstance m_Instance;
		QueueFamilyIndices m_QueueFamilyIndices;
		SwapChainSupportDetails m_SwapChainSupportDetails;
	};
}

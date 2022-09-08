#pragma once

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"

namespace Crystal {
	struct QueueFamilyIndices {
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool IsComplete() {
			return GraphicsFamily.has_value();
		}
	};

	class CRYSTAL_API VulkanPhysicalDevice {
	public:
		VulkanPhysicalDevice(VkInstance instance);
		~VulkanPhysicalDevice();

		inline VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }
		inline QueueFamilyIndices GetQueueFamilies() const { return m_QueueFamilyIndices;  }
		bool CheckPresentSupport(uint32_t queueFamilyIndex, VkSurfaceKHR surface) const;
		
		void AssignQueueFamilyIndicesWithPresent(VkSurfaceKHR surface);

		QueueFamilyIndices GetQueueFamilyWithPresent(
			VkPhysicalDevice device, VkSurfaceKHR surface) const {
			return FindQueueFamiliesWithPresent(device, surface);
		}
		
	private:
		static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		static QueueFamilyIndices FindQueueFamiliesWithPresent(VkPhysicalDevice device, VkSurfaceKHR surface);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		void SelectPhyiscalDevice();

		VkPhysicalDevice m_PhysicalDevice;
		VkInstance m_Instance;
		QueueFamilyIndices m_QueueFamilyIndices;
	};
}

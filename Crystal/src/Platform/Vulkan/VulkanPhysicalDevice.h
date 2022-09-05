#pragma once

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"

namespace Crystal {
	class CRYSTAL_API VulkanPhysicalDevice {
	public:
		struct QueueFamilyIndices {
			std::optional<uint32_t> GraphicsFamily;

			bool IsComplete() {
				return GraphicsFamily.has_value();
			}
		};
		
		VulkanPhysicalDevice(const VkInstance& instance);
		~VulkanPhysicalDevice();

		QueueFamilyIndices GetQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }
		
	private:
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		void SelectPhyiscalDevice();

		VkPhysicalDevice m_PhysicalDevice;
		VkInstance m_Instance;
	};
}
#pragma once

#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"

namespace Crystal {
	class CRYSTAL_API VulkanLogicalDevice {
	public:
		VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface);
		~VulkanLogicalDevice();

		VkDevice GetVkDevice() const { return m_Device; }
		VulkanPhysicalDevice* GetPhysicalDevice() const { return m_PhysicalDevice; }
		VkQueue GetQueue(QueueFlags flag) const;
		QueueFamilyIndex GetQueueIndex(QueueFlags flag) const;
		QueueFamilyIndices GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		void WaitGPU() const { vkDeviceWaitIdle(m_Device); }

	private:
		using Queues = std::array<VkQueue, sizeof(QueueFlags)>;

		VulkanPhysicalDevice* m_PhysicalDevice;

		QueueFamilyIndices m_QueueFamilyIndices;
		VkDevice m_Device;
		Queues m_Queues;
	};
}
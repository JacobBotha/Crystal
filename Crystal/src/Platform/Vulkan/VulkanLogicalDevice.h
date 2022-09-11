#pragma once

#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"

namespace Crystal {
	class CRYSTAL_API VulkanLogicalDevice {
	public:
		VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface);
		~VulkanLogicalDevice();

		VkDevice GetDevice() const{ return m_Device; }
		//VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		VkQueue GetQueue(QueueFlags flag);
		unsigned int GetQueueIndex(QueueFlags flag);

	private:
		using Queues = std::array<VkQueue, sizeof(QueueFlags)>;

		VulkanPhysicalDevice* m_PhysicalDevice;

		QueueFamilyIndices m_QueueFamilyIndices;
		VkDevice m_Device;
		Queues m_Queues;
	};
}
#pragma once

#include "VulkanPhysicalDevice.h"

namespace Crystal {
	class CRYSTAL_API VulkanLogicalDevice {
	public:
		VulkanLogicalDevice(
			std::unique_ptr<VulkanPhysicalDevice>& physicalDevice, 
			std::unique_ptr<VulkanInstance>& instance);
		~VulkanLogicalDevice();

		VkDevice GetDevice() { return m_Device; }
		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }

	private:
		VkDevice m_Device;
		VkQueue m_GraphicsQueue;
	};
}
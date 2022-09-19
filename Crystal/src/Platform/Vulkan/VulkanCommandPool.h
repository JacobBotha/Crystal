#pragma once

#include "VulkanLogicalDevice.h"
#include <vulkan/vulkan.h>

namespace Crystal {
	class VulkanCommandPool {
	public:
		VulkanCommandPool(VulkanLogicalDevice* device);
		~VulkanCommandPool();

		VulkanLogicalDevice* GetDevice() const { return m_Device; }
		VkCommandPool GetVkCommandPool() const { return m_CommandPool; }
	private:
		VulkanLogicalDevice* m_Device;
		VkCommandPool m_CommandPool;
	};
}

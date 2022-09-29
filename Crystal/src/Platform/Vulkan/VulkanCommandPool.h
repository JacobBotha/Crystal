#pragma once

#include "VulkanLogicalDevice.h"
#include <vulkan/vulkan.h>

namespace Crystal {
	class VulkanCommandPool 
	{
	public:
		VulkanCommandPool(VulkanLogicalDevice* device, bool transient = false);
		~VulkanCommandPool();

		VulkanLogicalDevice* GetDevice() const { return m_Device; }
		VkCommandPool GetVkCommandPool() const { return m_CommandPool; }

		void Reset();
	private:
		VulkanLogicalDevice* m_Device;
		VkCommandPool m_CommandPool;
	};
}

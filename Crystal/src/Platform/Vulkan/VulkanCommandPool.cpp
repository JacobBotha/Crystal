#include "clpch.h"

#include "VulkanCommandPool.h"

namespace Crystal {
	VulkanCommandPool::VulkanCommandPool(VulkanLogicalDevice* device)
		: m_Device(device) 
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = device->GetQueueIndex(QueueFlags::Graphics);

		VkResult err = vkCreateCommandPool(device->GetVkDevice(), &poolInfo, nullptr, &m_CommandPool);
		CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to create command pool!");
		(void)err;
	}

	VulkanCommandPool::~VulkanCommandPool() 
	{
		vkDestroyCommandPool(m_Device->GetVkDevice(), m_CommandPool, nullptr);
	}

	void VulkanCommandPool::Reset()
	{
		VkResult err = vkResetCommandPool(m_Device->GetVkDevice(), 
			m_CommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

		CL_CORE_ASSERT(err == VK_SUCCESS, "Could not reset command pool!");
	}

}

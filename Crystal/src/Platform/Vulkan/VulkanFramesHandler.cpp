#include "clpch.h"
#include "VulkanFramesHandler.h"

namespace Crystal {
	VulkanFramesHandler::VulkanFramesHandler(VulkanLogicalDevice* device, VulkanCommandPool* commandPool, uint16_t maxFrames)
		: m_Device(device),
		m_FrameIndex(0),
		m_TotalFrames(maxFrames)
	{
		m_ImageAvailableSemaphores.resize(m_TotalFrames);
		m_RenderFinishedSemaphores.resize(m_TotalFrames);
		m_InFlightFences.resize(m_TotalFrames);
		m_CommandBuffers.resize(m_TotalFrames);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint16_t i = 0; i < m_TotalFrames; i++)
		{

			VkResult err = vkCreateSemaphore(m_Device->GetVkDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]);
			CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to create semaphore!");
			err = vkCreateSemaphore(m_Device->GetVkDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]);
			CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to create semaphore!");
			err = vkCreateFence(m_Device->GetVkDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]);
			CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to create fence!");
			m_CommandBuffers[i] = std::make_unique<VulkanCommandBuffer>(commandPool);
		}
	}

	VulkanFramesHandler::~VulkanFramesHandler() 
	{
        m_Device->WaitGPU();
		for (uint16_t i = 0; i < m_TotalFrames; i++)
		{
			vkDestroySemaphore(m_Device->GetVkDevice(), m_RenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_Device->GetVkDevice(), m_ImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_Device->GetVkDevice(), m_InFlightFences[i], nullptr);
		}
	}
}
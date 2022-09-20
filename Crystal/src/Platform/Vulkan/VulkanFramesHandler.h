#pragma once

#include "VulkanCommandBuffer.h"

namespace Crystal {
	class VulkanFramesHandler {
	public:
		VulkanFramesHandler(VulkanLogicalDevice* device, VulkanCommandPool* commandPool, uint16_t maxFrames);
		~VulkanFramesHandler();

		VulkanCommandBuffer* GetCurrentCommandBuffer() { return m_CommandBuffers[m_FrameIndex].get(); }
		VkSemaphore GetCurrentRenderFinishedSemaphore() { return m_RenderFinishedSemaphores[m_FrameIndex]; }
		VkSemaphore GetCurrentImageAvailableSemaphore() { return m_ImageAvailableSemaphores[m_FrameIndex]; }
		VkFence GetCurrentInFlightFence() { return m_InFlightFences[m_FrameIndex]; }

		void IterFrame() { m_FrameIndex = (m_FrameIndex + 1) % m_TotalFrames; }
	private:
		VulkanLogicalDevice* m_Device;

		std::vector<std::unique_ptr<VulkanCommandBuffer>> m_CommandBuffers;
		std::vector<VkFence> m_InFlightFences;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;

		uint16_t m_FrameIndex;
		uint16_t m_TotalFrames;
	};
}

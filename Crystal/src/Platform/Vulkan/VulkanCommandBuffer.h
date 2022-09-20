#pragma once

#include "VulkanCommandPool.h"
#include "VulkanFramebuffer.h"

#include <vulkan/vulkan.h>

namespace Crystal {
	class VulkanCommandBuffer {
	public:
		enum class ResetFlags {
			None = 0,

		};

		struct RecordInfo {
			VulkanRenderPass::RenderPassPipeline pipelineType;
			bool dynamicState = true;
			VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
			VkViewport viewport;
			VkRect2D scissor;
		};
	public:
		VulkanCommandBuffer(VulkanCommandPool* commandPool);
		~VulkanCommandBuffer();

		void Record(VulkanFramebuffer* framebuffer, VkPipeline pipeline, RecordInfo& recordInfo);
		void Reset(ResetFlags flags = ResetFlags::None);
		VkCommandBuffer GetVkCommandBuffer() const { return m_CommandBuffer; }
		
	private:
		VulkanLogicalDevice* m_Device;

		VkCommandBuffer m_CommandBuffer;
	};
}

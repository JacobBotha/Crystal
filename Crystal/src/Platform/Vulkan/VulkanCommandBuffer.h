#pragma once

#include "VulkanCommandPool.h"
#include "VulkanFramebuffer.h"
#include "VulkanBuffer.h"

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
			VkViewport viewport{};
			VkRect2D scissor{};
			VkExtent2D extent{};
		};
	public:
		VulkanCommandBuffer(VulkanCommandPool* commandPool, bool primary);
		~VulkanCommandBuffer();

		void Begin();
		void End();
		void Record(VulkanFramebuffer* framebuffer, VkPipeline pipeline, RecordInfo& recordInfo, VulkanBuffer* vertexBuffer, uint32_t size);
		void Reset(ResetFlags flags = ResetFlags::None);
		VkCommandBuffer GetVkCommandBuffer() const { return m_CommandBuffer; }
		
	private:
		VulkanLogicalDevice* m_Device;

		VkCommandBuffer m_CommandBuffer;
		bool m_Primary;
	};
}

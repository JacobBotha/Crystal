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
	public:
		VulkanCommandBuffer(VulkanCommandPool* commandPool);
		~VulkanCommandBuffer();

		void Record(VulkanFramebuffer* framebuffer, 
			VkPipeline pipeline, 
			VulkanRenderPass::RenderPassPipeline pipelineType, 
			bool dynamicState);
		void Reset(ResetFlags flags = ResetFlags::None);
		VkCommandBuffer GetVkCommandBuffer() const { return m_CommandBuffer; }
		
	private:
		VulkanLogicalDevice* m_Device;

		VkCommandBuffer m_CommandBuffer;
	};
}

#include "clpch.h"

#include "VulkanCommandBuffer.h"

namespace Crystal {
	VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandPool* commandPool)
		: m_Device(commandPool->GetDevice())
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool->GetVkCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		VkResult err = vkAllocateCommandBuffers(m_Device->GetVkDevice(), &allocInfo, &m_CommandBuffer);
	}

	void VulkanCommandBuffer::Record(VulkanFramebuffer* framebuffer, VkPipeline pipeline, RecordInfo& recordInfo) 
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkResult err = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
		CL_CORE_ASSERT(err == VK_SUCCESS, "Could not begine command buffer!");

		VulkanRenderPass* renderPass = framebuffer->GetVulkanRenderPass();
		VkExtent2D extent = recordInfo.extent;

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass->GetVkRenderPass();
		renderPassInfo.framebuffer = framebuffer->GetVkFramebuffer();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extent;

		VkClearValue clearColor = recordInfo.clearColor;
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		if (recordInfo.pipelineType == VulkanRenderPass::RenderPassPipeline::Graphics)
			pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		else CL_CORE_ERROR("Only Graphics Pipeline is supported!");
					
		vkCmdBindPipeline(m_CommandBuffer, pipelineBindPoint, pipeline);

		if (recordInfo.dynamicState) {
			vkCmdSetViewport(m_CommandBuffer, 0, 1, &recordInfo.viewport);

			vkCmdSetScissor(m_CommandBuffer, 0, 1, &recordInfo.scissor);
		}

		vkCmdDraw(m_CommandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(m_CommandBuffer);

		err = vkEndCommandBuffer(m_CommandBuffer);
		CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to end command buffer!");
		(void)err;
	}

	void VulkanCommandBuffer::Reset(ResetFlags flags)
	{
		VkCommandBufferResetFlags resetFlags;
		switch (flags)
		{
		case Crystal::VulkanCommandBuffer::ResetFlags::None:
			resetFlags = 0;
			break;
		default:
			resetFlags = 0;
			break;
		}

		vkResetCommandBuffer(m_CommandBuffer, resetFlags);
	}

	VulkanCommandBuffer::~VulkanCommandBuffer() {}
}
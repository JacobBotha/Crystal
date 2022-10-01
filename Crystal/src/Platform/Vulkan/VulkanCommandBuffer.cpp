#include "clpch.h"

#include "VulkanCommandBuffer.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "Crystal/ImGui/ImGuiLayer.h"

namespace Crystal {
	VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandPool* commandPool,
		bool primary, bool freeOnDestroy)
		: m_Device(commandPool->GetDevice()),
		m_CommandPool(commandPool),
		m_Primary(primary),
		m_FreeOnDestroy(freeOnDestroy)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool->GetVkCommandPool();
		allocInfo.commandBufferCount = 1;
		if (m_Primary) 
		{
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		}
		else 
		{
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		}

		VkResult err = vkAllocateCommandBuffers(m_Device->GetVkDevice(), 
			&allocInfo, &m_CommandBuffer);
	}

	VulkanCommandBuffer::~VulkanCommandBuffer() 
	{
		if (m_FreeOnDestroy)
			vkFreeCommandBuffers(m_Device->GetVkDevice(), m_CommandPool->GetVkCommandPool(), 1, &m_CommandBuffer);
	}

	void VulkanCommandBuffer::Begin(bool oneTimeSubmit)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (oneTimeSubmit)
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkResult err = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
		CL_CORE_ASSERT(err == VK_SUCCESS, "Could not begine command buffer!");
		(void)err;
	}

	void VulkanCommandBuffer::End()
	{
		VkResult err = vkEndCommandBuffer(m_CommandBuffer);
		CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to end command buffer!");
		(void)err;
	}

	//This function needs to be removed/split up.
	void VulkanCommandBuffer::Record(VulkanFramebuffer* framebuffer, 
		VkPipeline pipeline, RecordInfo& recordInfo, VulkanBuffer* vertexBuffer, uint32_t size, VulkanBuffer* indexBuffer, uint32_t indexCount) 
	{
		Begin();

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

		vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, 
			VK_SUBPASS_CONTENTS_INLINE);

		VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		if (recordInfo.pipelineType == VulkanRenderPass::RenderPassPipeline::Graphics)
			pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		else CL_CORE_ERROR("Only Graphics Pipeline is supported!");
					
		vkCmdBindPipeline(m_CommandBuffer, pipelineBindPoint, pipeline);

		if (recordInfo.dynamicState) {
			vkCmdSetViewport(m_CommandBuffer, 0, 1, &recordInfo.viewport);

			vkCmdSetScissor(m_CommandBuffer, 0, 1, &recordInfo.scissor);
		}

		if (vertexBuffer)
		{
			VkBuffer vertexBuffers[] = { vertexBuffer->GetVkBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, vertexBuffers, offsets);
			
			vkCmdBindIndexBuffer(m_CommandBuffer, indexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT16);

			vkCmdDrawIndexed(m_CommandBuffer, indexCount, 1, 0, 0, 0);
		}

		ImGui_ImplVulkan_RenderDrawData((ImDrawData*)ImGuiLayer::GetDrawData(), m_CommandBuffer);

		vkCmdEndRenderPass(m_CommandBuffer);

		End();
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
}
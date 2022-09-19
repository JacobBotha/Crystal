#include "clpch.h"

#include "VulkanRenderPass.h"

namespace Crystal {
	VulkanRenderPass::VulkanRenderPass(VulkanLogicalDevice* device, 
		VulkanSwapChain* swapChain, 
		RenderPassPipeline pipeline) 
		: m_Device(device), 
		m_RenderPass(VK_NULL_HANDLE),
		m_SwapChain(swapChain)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChain->GetVkImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkSubpassDescription subpass{};
		switch (pipeline)
		{
		case Crystal::VulkanRenderPass::RenderPassPipeline::Graphics:
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			break;
		case Crystal::VulkanRenderPass::RenderPassPipeline::Compute:
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
			break;
		default:
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			break;
		}
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkResult err = vkCreateRenderPass(m_Device->GetVkDevice(), &renderPassInfo, nullptr, &m_RenderPass);
		CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to create render pass!");
	}

	VulkanRenderPass::~VulkanRenderPass() {
		vkDestroyRenderPass(m_Device->GetVkDevice(), m_RenderPass, nullptr);
	}
}
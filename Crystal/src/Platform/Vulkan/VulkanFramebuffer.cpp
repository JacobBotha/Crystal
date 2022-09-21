#include "clpch.h"

#include "VulkanFramebuffer.h"

namespace Crystal {
	VulkanFramebuffer::VulkanFramebuffer(VulkanLogicalDevice* device, 
			VulkanRenderPass* renderPass, 
			VkExtent2D swapChainExtent, 
			VkImageView imageView)
        : m_Device(device),
        m_RenderPass(renderPass),
        m_Framebuffer(VK_NULL_HANDLE)
	{
        VkImageView attachments[] = {
           imageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass->GetVkRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        VkResult err = vkCreateFramebuffer(device->GetVkDevice(), &framebufferInfo, nullptr, &m_Framebuffer);
        CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to create framebuffer!");
	}

	VulkanFramebuffer::~VulkanFramebuffer() {
        vkDestroyFramebuffer(m_Device->GetVkDevice(), m_Framebuffer, nullptr);
    }
}
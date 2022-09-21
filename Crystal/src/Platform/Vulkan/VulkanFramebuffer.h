#pragma once

#include "VulkanLogicalDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"

#include <vulkan/vulkan.h>

namespace Crystal {
	class VulkanFramebuffer {
	public:
		VulkanFramebuffer(VulkanLogicalDevice* device, 
			VulkanRenderPass* renderPass, 
			VkExtent2D swapChainExtent, 
			VkImageView imageView);
		~VulkanFramebuffer();

		VulkanRenderPass* GetVulkanRenderPass() const{ return m_RenderPass; }
		VkFramebuffer GetVkFramebuffer() const { return m_Framebuffer; }

	private:
		VulkanLogicalDevice* m_Device;
		VulkanRenderPass* m_RenderPass;

		VkFramebuffer m_Framebuffer;
	};
}

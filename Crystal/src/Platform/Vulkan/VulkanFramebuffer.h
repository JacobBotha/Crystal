#pragma once

#include "VulkanLogicalDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"

#include <vulkan/vulkan.h>

namespace Crystal {
	class VulkanFramebuffer {
	public:
		VulkanFramebuffer(VulkanLogicalDevice* device, 
			std::shared_ptr<VulkanRenderPass> renderPass, 
			VkExtent2D swapChainExtent, 
			VkImageView imageView);
		~VulkanFramebuffer();

		VulkanRenderPass* GetVulkanRenderPass() const{ return m_RenderPass.get(); }
		VkFramebuffer GetVkFramebuffer() const { return m_Framebuffer; }

	private:
		VulkanLogicalDevice* m_Device;
		std::shared_ptr<VulkanRenderPass> m_RenderPass;

		VkFramebuffer m_Framebuffer;
	};
}

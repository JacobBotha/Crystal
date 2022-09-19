#pragma once

#include "VulkanLogicalDevice.h"
#include "VulkanSwapChain.h"

#include <vulkan/vulkan.h>

namespace Crystal {
	class CRYSTAL_API VulkanRenderPass {
	public:
		enum class RenderPassPipeline {
			Graphics,
			Compute
		};
	public:
		VulkanRenderPass(
			VulkanLogicalDevice* device, 
			VulkanSwapChain* swapChain, 
			RenderPassPipeline pipeline = RenderPassPipeline::Graphics);
		~VulkanRenderPass();

		VulkanSwapChain* GetVulkanSwapChain() const { return m_SwapChain; }
		VkRenderPass GetVkRenderPass() const { return m_RenderPass; }
	private:
		VulkanLogicalDevice* m_Device;
		VulkanSwapChain* m_SwapChain;
		VkRenderPass m_RenderPass;
	};
}

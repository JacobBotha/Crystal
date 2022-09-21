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

		VkRenderPass GetVkRenderPass() const { return m_RenderPass; }
	private:
		VkRenderPass m_RenderPass;
		VulkanLogicalDevice* m_Device;
	};
}

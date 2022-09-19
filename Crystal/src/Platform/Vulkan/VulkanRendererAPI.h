#pragma once

#include "Crystal/Renderer/RendererAPI.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"

namespace Crystal {
	class CRYSTAL_API VulkanRendererAPI : RendererAPI {
	public:
		VulkanRendererAPI();
		~VulkanRendererAPI();

		virtual void Init() override;
		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void CreateGraphicsPipeline(GraphicsPipelineCreateInfo createInfo) override;
		virtual void DrawFrame();
		void DestroyGraphicsPipeline();

		VulkanLogicalDevice* GetLogicalDevice() const { return m_LogicalDevice.get(); }

		//void CreateGraphicsPipeline

	private:
		void CreateSyncObjects();
		void DestroySyncObjects();

		std::unique_ptr<VulkanInstance> m_Instance;
		std::unique_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		std::unique_ptr<VulkanSurface> m_Surface;

		std::unique_ptr<VulkanLogicalDevice> m_LogicalDevice;
		std::unique_ptr<VulkanSwapChain> m_SwapChain;
		std::shared_ptr<VulkanRenderPass> m_RenderPass;
		std::vector<std::unique_ptr<VulkanFramebuffer>> m_Framebuffers;
		std::unique_ptr<VulkanCommandPool> m_CommandPool;
		std::unique_ptr<VulkanCommandBuffer> m_CommandBuffer;

		VkFence m_InFlightFence;
		VkSemaphore m_RenderFinishedSemaphore;

		VkPipelineLayout m_GraphicsPipelineLayout;
		VkPipeline m_GraphicsPipeline;
		

	};
}

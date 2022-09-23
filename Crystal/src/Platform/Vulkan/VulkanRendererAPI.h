#pragma once

#include "Crystal/Renderer/RendererAPI.h"
#include "Crystal/Events/ApplicationEvent.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanFramesHandler.h"

namespace Crystal {
	class CRYSTAL_API VulkanRendererAPI : RendererAPI {
	public:
		VulkanRendererAPI();
		~VulkanRendererAPI();

		virtual void Init() override;
		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void WindowResized(WindowResizeEvent& e) override 
		{
			m_Surface->UpdateFramebufferSize();
			m_FramebufferResized = true;
		}

		virtual void CreateGraphicsPipeline(GraphicsPipelineCreateInfo createInfo) override;
		virtual void DrawFrame();
		virtual void PresentFrame();

		VulkanInstance* GetInstance() const { return m_Instance.get(); }
		VulkanPhysicalDevice* GetPhysicalDevice () const { return m_PhysicalDevice.get(); }
		//VulkanCommandBuffer* GetCurrentCommandBuffer() const { return m_Frames->GetCurrentCommandBuffer(); }
		VulkanLogicalDevice* GetLogicalDevice() const { return m_LogicalDevice.get(); }
		VulkanSwapChain* GetSwapChain() const { return m_SwapChain.get(); }
		VulkanRenderPass* GetRenderPass() const { return m_RenderPass.get(); }
		VulkanCommandPool* GetCommandPool() const { return m_CommandPool.get(); }
		VulkanFramebuffer* GetCurrentFramebuffer() const { return m_Framebuffers[m_CurrentImageIndex].get(); }
		VkSemaphore GetCurrentImageAvailableSemaphore() const { return m_Frames->GetCurrentImageAvailableSemaphore(); }
		VkFence GetCurrentInFlightFence() const { return m_Frames->GetCurrentInFlightFence(); }

		//void CreateGraphicsPipeline

	private:
		void DestroyGraphicsPipeline();
		void RecreateSwapChain();
		void CreateFramebuffers();
		VkViewport CreateViewport(float x, float y, float width, float height);
		VkRect2D CreateScissor(VkOffset2D offset, VkExtent2D extent);
		void InitRecordInfo();

		std::unique_ptr<VulkanInstance> m_Instance;
		std::unique_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		std::unique_ptr<VulkanSurface> m_Surface;

		std::unique_ptr<VulkanLogicalDevice> m_LogicalDevice;
		std::unique_ptr<VulkanSwapChain> m_SwapChain;
		std::unique_ptr<VulkanRenderPass> m_RenderPass;
		std::vector<std::unique_ptr<VulkanFramebuffer>> m_Framebuffers;
		std::unique_ptr<VulkanCommandPool> m_CommandPool;
		std::unique_ptr<VulkanFramesHandler> m_Frames;

		VkPipelineLayout m_GraphicsPipelineLayout;
		VkPipeline m_GraphicsPipeline;

		VulkanCommandBuffer::RecordInfo m_RecordInfo;
		//VkClearValue m_ClearValue;
		
		bool m_FramebufferResized;
		bool m_PresentReady;
		uint32_t m_CurrentImageIndex;

	};
}

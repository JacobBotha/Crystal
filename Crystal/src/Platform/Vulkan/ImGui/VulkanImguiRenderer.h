#pragma once

#include "Crystal/ImGui/ImGuiRenderer.h"
#include "Crystal/Core/Window.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Crystal {
	class VulkanImGuiRenderer : public ImGuiRenderer {
	public:
		VulkanImGuiRenderer();
		~VulkanImGuiRenderer();

		virtual void Init(RendererAPI* rendererAPI, Window* window) override;
		virtual void NewFrame() override;
		virtual void Shutdown() override;
		virtual void Render(void* drawData) override;
	private:
		VulkanRendererAPI* m_RendererAPI;
		//std::unique_ptr<VulkanFramesHandler> m_Frames;
		std::vector<std::unique_ptr<VulkanCommandBuffer>> m_CommandBuffers;
		uint32_t m_CommandBufferIndex;
		Window* m_Window;
		VkDescriptorPool m_Pool;
	};
}

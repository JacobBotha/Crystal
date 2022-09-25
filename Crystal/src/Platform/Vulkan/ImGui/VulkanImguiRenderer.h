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
		Window* m_Window;
		VkDescriptorPool m_Pool;
	};
}

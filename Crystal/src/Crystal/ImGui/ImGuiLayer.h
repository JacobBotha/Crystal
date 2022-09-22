#pragma once

#include "Crystal/Core/Layer.h"
#include "Platform/Vulkan/ImGui/VulkanImGuiRenderer.h"

namespace Crystal {
	class CRYSTAL_API ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnUpdate() override;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override {}

		inline static void* GetDrawData() { return s_DrawData; }

		static std::unique_ptr<ImGuiRenderer> s_ImGuiRenderer;
	private:
		float m_Time = 0.0f;
		inline static void* s_DrawData;
	};
}

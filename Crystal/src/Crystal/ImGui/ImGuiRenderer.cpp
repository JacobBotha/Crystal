#include "clpch.h"
#include "ImGuiRenderer.h"
#include "Crystal/Renderer/Renderer.h"
#include "Platform/Vulkan/ImGui/VulkanImguiRenderer.h"

namespace Crystal {
	std::unique_ptr<ImGuiRenderer> ImGuiRenderer::Create() 
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			CL_CORE_ASSERT(false, "Currently no support for None renderer!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return std::make_unique<VulkanImGuiRenderer>();
		}
			
		return nullptr;
	}
}
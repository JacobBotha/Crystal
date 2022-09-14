#include "clpch.h"

#include "Shader.h"
#include "Renderer.h"
#include "Platform/Vulkan/VulkanShader.h"
//#incldue "RendererAPI.h"

namespace Crystal {
	std::shared_ptr<Shader> Shader::Create(const std::string& fileName) {
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    CL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::Vulkan:  return std::make_shared<VulkanShader>(fileName);
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
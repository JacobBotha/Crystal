#include "clpch.h"

#include "Shader.h"
#include "Renderer.h"
#include "Platform/Vulkan/VulkanShader.h"
//#incldue "RendererAPI.h"

namespace Crystal {
	std::shared_ptr<Shader> Shader::Create(const std::string fileName, RendererAPI* rAPI) {
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    CL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::Vulkan:  return std::make_shared<VulkanShader>(fileName, rAPI);
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::string Shader::GetNameFromFile(const std::string& fileName) {
		// Extract name from filepath
		auto lastSlash = fileName.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = fileName.rfind('.');
		auto count = lastDot == std::string::npos ? fileName.size() - lastSlash : lastDot - lastSlash;
		return fileName.substr(lastSlash, count);
	}
}
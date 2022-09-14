#pragma once

#include "Crystal/Renderer/Shader.h"

#include <vulkan/vulkan.h>

namespace Crystal {
	class CRYSTAL_API VulkanShader : public Shader {
	public:
		VulkanShader(const std::string& fileName);
		~VulkanShader();
	private:
		VkShaderModule m_ShaderModule;

	};
}

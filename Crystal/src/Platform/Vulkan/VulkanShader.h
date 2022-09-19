#pragma once

#include "VulkanLogicalDevice.h"
#include "Crystal/Renderer/Shader.h"
//#include "Crystal/Renderer/RendererAPI.h"

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>


namespace Crystal {
	class CRYSTAL_API VulkanShader : public Shader {
	public:
		VulkanShader(const std::string fileName);
		~VulkanShader();

		virtual void CompileOrGetBinaries() override;

		std::vector<uint32_t> ReadFile(const std::string& fileName) const;
		std::string ReadTextFile(const std::string_view& fileName);
		std::string PreprocessShader(const std::string& fileName, const std::string& source, shaderc_shader_kind shaderKind);
		inline const std::vector<uint32_t>& GetSPIRV() const { return m_SPIRV; }
	private:
		shaderc::Compiler m_Compiler;
		shaderc::CompileOptions m_CompileOptions;

		std::vector<uint32_t> m_SPIRV;
	};
}

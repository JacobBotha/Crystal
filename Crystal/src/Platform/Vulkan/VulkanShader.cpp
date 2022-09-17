#include "clpch.h"
#include "VulkanShader.h"
#include "VulkanRendererAPI.h"

#include <fstream>
#include <ctime>

namespace Crystal {
	namespace Utils {
		static const std::string CacheDirectory = "assets/cache/shader/vulkan";

		static void CreateCacheDirectory() {
			std::string cacheDirectory = CacheDirectory;
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static bool IsSPIRV(const std::string_view& fileName) {
			const std::string_view extention = fileName.substr(fileName.size() - 4, 4);
			return extention == ".spv";
		}

		static shaderc_shader_kind InferKindFromFileName(
				const std::string_view& fileName) {
			// Get file extension
			const auto dotIndex = fileName.find_last_of('.');
			const auto extension = fileName.substr(dotIndex + 1);

			if (extension == "comp")
				return shaderc_shader_kind::shaderc_glsl_default_compute_shader;
			if (extension == "vert")
				return shaderc_shader_kind::shaderc_glsl_default_vertex_shader;
			if (extension == "frag")
				return shaderc_shader_kind::shaderc_glsl_default_fragment_shader;
			if (extension == "rchit")
				return shaderc_shader_kind::shaderc_glsl_default_closesthit_shader;
			if (extension == "rgen")
				return shaderc_shader_kind::shaderc_glsl_default_raygen_shader;
			if (extension == "rmiss")
				return shaderc_shader_kind::shaderc_glsl_default_miss_shader;
			if (extension == "rahit")
				return shaderc_shader_kind::shaderc_glsl_default_anyhit_shader;
			if (extension == "geom")
				return shaderc_shader_kind::shaderc_glsl_default_geometry_shader;
			if (extension == "mesh")
				return shaderc_shader_kind::shaderc_glsl_default_mesh_shader;
			if (extension == "rcall")
				return shaderc_shader_kind::shaderc_glsl_default_callable_shader;

			return shaderc_shader_kind::shaderc_glsl_infer_from_source;
		}

		static std::string GetCachedFileExtention(shaderc_shader_kind kind) {
			switch (kind)
			{
			case shaderc_shader_kind::shaderc_glsl_default_fragment_shader:
				return ".cached_vulkan.frag";
			case shaderc_shader_kind::shaderc_glsl_default_vertex_shader:
				return ".cached_vulkan.vert";
			default:
				break;
			}

			return ".cached_vulkan.spv";
		}

		static std::vector<char> ReadFile(const std::string& filename) {
			std::ifstream file(filename, std::ios::ate | std::ios::binary);

			if (!file.is_open()) {
				throw std::runtime_error("failed to open file!");
			}

			size_t fileSize = (size_t)file.tellg();
			std::vector<char> buffer(fileSize);
			file.seekg(0);
			file.read(buffer.data(), fileSize);
			file.close();

			return buffer;
		}
	}

	VulkanShader::VulkanShader(const std::string fileName, RendererAPI* rAPI) 
		: Shader(fileName),
		m_Device(((VulkanRendererAPI*)rAPI)->GetLogicalDevice()), 
		m_ShaderModule(VK_NULL_HANDLE) 
	{

		Utils::CreateCacheDirectory();

		//Setup compiler environment
		m_CompileOptions.SetSourceLanguage(shaderc_source_language_glsl);
		m_CompileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
		
		if (!Utils::IsSPIRV(m_FileName))
		{
			clock_t t;
			t = clock();
			CompileOrGetBinaries();
			t = clock() - t;
			CL_CORE_WARN("Shader creation took {0} seconds", ((float)t) / CLOCKS_PER_SEC);


			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = m_SPIRV.size() * sizeof(unsigned int);
			createInfo.pCode = m_SPIRV.data();
			VkResult err = vkCreateShaderModule(m_Device->GetVkDevice(), &createInfo, nullptr, &m_ShaderModule);
			CL_CORE_ASSERT(err == VK_SUCCESS, "Could not create vulkan shader module!");
		}
		else {
			const std::vector<char> code = Utils::ReadFile(m_FileName);
			m_SPIRV = std::vector<uint32_t>();

			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

			VkResult err = vkCreateShaderModule(m_Device->GetVkDevice(), &createInfo, nullptr, &m_ShaderModule);
			CL_CORE_ASSERT(err != VK_SUCCESS, "Failed to create shader module!");
		}
	}

	VulkanShader::~VulkanShader() {
		vkDestroyShaderModule(m_Device->GetVkDevice(), m_ShaderModule, nullptr);
	}

	std::vector<uint32_t> VulkanShader::ReadFile(const std::string& fileName) const
	{
		std::ifstream fileStream(fileName.data(), std::ios::binary | std::ios::in | std::ios::ate);
		if (!fileStream.is_open())
			CL_CORE_ERROR("Could not open file '{0}'", fileName);

		const size_t fileSize = fileStream.tellg();
		fileStream.seekg(0);
		std::vector<uint32_t> buffer(fileSize);
		fileStream.read((char*)buffer.data(), fileSize);
		fileStream.close();
		return buffer;
	}

	std::string VulkanShader::ReadTextFile(const std::string_view & fileName) {
		std::string buffer;
		std::ifstream fileStream(fileName.data());
		CL_CORE_ASSERT(fileStream.is_open(), "Could not open file.");

		std::string temp;
		while (getline(fileStream, temp))
			buffer.append(temp), buffer.append("\n");

		fileStream >> buffer;
		fileStream.close();
		return buffer;
	}

	std::string VulkanShader::PreprocessShader(const std::string& fileName, const std::string& source, shaderc_shader_kind shaderKind)
	{
		auto result = m_Compiler.PreprocessGlsl(source, shaderKind, fileName.data(), m_CompileOptions);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			CL_CORE_ERROR("%s", result.GetErrorMessage().data());
			return "";
		}
		return { result.cbegin(), result.cend() };
	}


	//Potentially add caching in future
	void VulkanShader::CompileOrGetBinaries() {
		const shaderc_shader_kind kind = Utils::InferKindFromFileName(m_FileName);				  // Infer shader kind
		const std::string source = ReadTextFile(m_FileName);

		const std::string result = PreprocessShader(m_FileName, source, kind);
		shaderc::SpvCompilationResult module = m_Compiler.CompileGlslToSpv(result, kind, m_FileName.c_str(), m_CompileOptions);

		CL_CORE_ASSERT(module.GetCompilationStatus() == shaderc_compilation_status_success, module.GetErrorMessage());

		m_SPIRV = std::vector(module.cbegin(), module.cend());
	}
}
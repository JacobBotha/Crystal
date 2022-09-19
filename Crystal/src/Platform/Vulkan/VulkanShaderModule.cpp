#include "clpch.h"

#include "VulkanShaderModule.h"

namespace Crystal {
	namespace VulkanShaderModule {
		VkShaderModule VulkanShaderModule::Create(VulkanShader* shader, VulkanLogicalDevice* device) {
			VkShaderModule shaderModule;

			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = shader->GetSPIRV().size() * sizeof(unsigned int);
			createInfo.pCode = shader->GetSPIRV().data();
			
			VkResult err = vkCreateShaderModule(device->GetVkDevice(), &createInfo, nullptr, &shaderModule);
			CL_CORE_ASSERT(err == VK_SUCCESS, "Could not create vulkan shader module!");
			(void*)err;

			return shaderModule;
		}
		
		void Destroy(VkShaderModule shaderModule, VulkanLogicalDevice* device) {
			vkDestroyShaderModule(device->GetVkDevice(), shaderModule, nullptr);
		}
	}
}

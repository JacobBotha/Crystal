#pragma once

#include "VulkanShader.h"

#include "vulkan/vulkan.h"

namespace Crystal {
	namespace VulkanShaderModule {
		VkShaderModule Create(VulkanShader* shader, VulkanLogicalDevice* device);
		
		void Destroy(VkShaderModule shaderModule, VulkanLogicalDevice* device);
	}
}

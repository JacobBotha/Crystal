#pragma once

#include "Crystal/Renderer/Vertex.h"
#include <vulkan/vulkan.h>

namespace Crystal
{
	namespace VulkanVertex
	{
		VkVertexInputBindingDescription GetBindingDescription();

		std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
	}
}

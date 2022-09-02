#pragma once

#include "vulkan/vulkan.h"

namespace Crystal {
	class CRYSTAL_API VulkanInstance {
	public:
		VulkanInstance();
		~VulkanInstance();
		
	private:
		VkInstance m_Instance;
	};
}

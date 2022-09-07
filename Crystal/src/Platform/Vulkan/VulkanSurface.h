#pragma once

#include "Crystal/Core/Window.h"

#include <vulkan/vulkan.h>

namespace Crystal {
	class CRYSTAL_API VulkanSurface {
	public:
		VulkanSurface(Window* window);
		~VulkanSurface();
	private:
		Window* m_Window;
		VkSurfaceKHR surface;
	};
}

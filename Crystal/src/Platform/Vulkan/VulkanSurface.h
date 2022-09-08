#pragma once

#include "Crystal/Core/Window.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"

#include <vulkan/vulkan.h>

namespace Crystal {
	class CRYSTAL_API VulkanSurface {
	public:
		VulkanSurface(Window* window, VulkanInstance* instance, VulkanPhysicalDevice* physicalDevice);
		~VulkanSurface();

		VkSurfaceKHR GetVkSurface() const { return m_Surface; }
	private:
		VkSurfaceKHR m_Surface;

		Window* m_Window;
		VulkanInstance* m_Instance;
	};
}

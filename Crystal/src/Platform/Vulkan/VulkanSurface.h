#pragma once

#include "Crystal/Core/Window.h"
#include "Crystal/Renderer/QueueFlags.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"

#include <vulkan/vulkan.h>

namespace Crystal {
	class CRYSTAL_API VulkanSurface {
	public:
		VulkanSurface(Window* window, VulkanInstance* instance);
		~VulkanSurface();

		bool CanPresent(VulkanPhysicalDevice* physicalDevice, QueueFamilyIndex index) const;
		VkSurfaceKHR GetVkSurface() const { return m_Surface; }
		//Get the width and height (in that order) of the frame buffer in pixels.
		std::pair<int, int> GetFramebufferSize() const { return std::pair<int, int>(m_Width, m_Height); }
		//Update dimension if window is resized
		void UpdateFramebufferSize() { GetNativeFramebufferSize(); }
	private:
		void GetNativeFramebufferSize();
		Window* m_Window;
		VulkanInstance* m_Instance;

		VkSurfaceKHR m_Surface;

		int m_Width;
		int m_Height;
	};
}

#include "clpch.h"

#include "VulkanSurface.h"
#include <GLFW/glfw3.h>

namespace Crystal {
	VulkanSurface::VulkanSurface(Window* window, VulkanInstance* instance) 
		: m_Window(window), m_Instance(instance), m_Surface(VK_NULL_HANDLE) {

		VkResult result = glfwCreateWindowSurface(m_Instance->GetVkInstance(), 
			(GLFWwindow*)m_Window->GetNativeWindow(), nullptr, &m_Surface);
		
		CL_CORE_ASSERT(result == VK_SUCCESS, "Could not create Vulkan surface!");

	}

	bool VulkanSurface::CanPresent(VulkanPhysicalDevice* physicalDevice, QueueFamilyIndex index) {
		return physicalDevice->CheckPresentSupport(index, m_Surface);
	}

	VulkanSurface::~VulkanSurface() {
		vkDestroySurfaceKHR(m_Instance->GetVkInstance(), m_Surface, nullptr);
	}
}
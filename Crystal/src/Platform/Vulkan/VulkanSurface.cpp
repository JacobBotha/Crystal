#include "clpch.h"

#include "VulkanSurface.h"
#include <GLFW/glfw3.h>

namespace Crystal {
	VulkanSurface::VulkanSurface(Window* window, VulkanInstance* instance) 
		: m_Window(window), m_Instance(instance) {

		VkResult result = glfwCreateWindowSurface(
			m_Instance->GetInstance(), 
			(GLFWwindow*)m_Window->GetNativeWindow(), 
			nullptr, &m_Surface);
		
		CL_CORE_ASSERT(result == VK_SUCCESS, 
			"Could not create Vulkan surface");
	}

	VulkanSurface::~VulkanSurface() {
		vkDestroySurfaceKHR(m_Instance->GetInstance(), m_Surface, nullptr);
	}
}
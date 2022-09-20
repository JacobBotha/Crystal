#include "clpch.h"

#include "VulkanSurface.h"
#include <GLFW/glfw3.h>

namespace Crystal {
	VulkanSurface::VulkanSurface(Window* window, VulkanInstance* instance) 
		: m_Window(window), m_Instance(instance), m_Surface(VK_NULL_HANDLE) {

		GLFWwindow* wd = (GLFWwindow*)m_Window->GetNativeWindow();

		VkResult result = glfwCreateWindowSurface(m_Instance->GetVkInstance(), 
			wd, nullptr, &m_Surface);

		glfwGetFramebufferSize(wd, &m_Width, &m_Height);
		
		CL_CORE_ASSERT(result == VK_SUCCESS, "Could not create Vulkan surface!");

	}

	bool VulkanSurface::CanPresent(VulkanPhysicalDevice* physicalDevice, QueueFamilyIndex index) const {
		return physicalDevice->CheckPresentSupport(index, m_Surface);
	}

	VulkanSurface::~VulkanSurface() {
		vkDestroySurfaceKHR(m_Instance->GetVkInstance(), m_Surface, nullptr);
	}
	
	void VulkanSurface::GetNativeFramebufferSize() {
		GLFWwindow* wd = (GLFWwindow*)m_Window->GetNativeWindow();

		glfwGetFramebufferSize(wd, &m_Width, &m_Height);
	}
}
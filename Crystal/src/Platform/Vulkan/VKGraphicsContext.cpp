#include "clpch.h"
#include "VKGraphicsContext.h"

namespace Crystal {
	GraphicsContext* GraphicsContext::Create(void* window) {
		return new VKGraphicsContext((GLFWwindow*) window);
	}

	VKGraphicsContext::VKGraphicsContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle) {}

	void VKGraphicsContext::Init() {
		if (glfwVulkanSupported()) {
			CL_CORE_INFO("Vulkan is supported!");
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	void VKGraphicsContext::SwapBuffers() {
		//SwapBuffers is not required for vulkan
		return;
	}
}
#pragma once
#include "Crystal/GraphicsContext.h"

//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Crystal {
	class VKGraphicsContext : public GraphicsContext {
	public:
		VKGraphicsContext(GLFWwindow* Window);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		void* m_WindowHandle;
	};
}



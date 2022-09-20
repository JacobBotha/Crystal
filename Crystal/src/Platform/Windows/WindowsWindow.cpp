#include "clpch.h"
#include "WindowsWindow.h"
#include "Crystal/Events/ApplicationEvent.h"
#include "Crystal/Events/KeyEvent.h"
#include "Crystal/Events/MouseEvent.h"
//#include "Platform/Vulkan/VKGraphicsContext.h"

namespace Crystal {
	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description) {
		CL_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProperties& props) 
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& props) 
	{
		Init(props);
	}


	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProperties& props)
	{
		m_Data.Title = props.Title;
		m_Data.Height = props.Height;
		m_Data.Width = props.Width;

		//Initialise GLFW if it is not already
		if (s_GLFWWindowCount == 0) 
		{
			if (!glfwInit()) 
			{
				// TODO: Handle initialization failure
				int code = glfwGetError(NULL);

				CL_CORE_ASSERT(code, "GLFW Error: Could not initialise GLFW")
			}

			glfwSetErrorCallback(GLFWErrorCallback);
			//Create a Vulkan initialisation object
			m_GraphicsContext = GraphicsContext::Create(m_Window);
			m_GraphicsContext->Init();
		
		}


		m_Window = glfwCreateWindow(m_Data.Height, m_Data.Width, m_Data.Title.c_str(), NULL, NULL);
		s_GLFWWindowCount++;
		glfwSetWindowUserPointer(m_Window, &m_Data);
		//SetVSync(true);


		//Set event callbacks
		glfwSetWindowCloseCallback(m_Window, [] (GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event((float)width, (float)height);
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			if (action == GLFW_RELEASE) {
				KeyReleasedEvent event(key);
				data.EventCallback(event);
			} 
			if (action == GLFW_REPEAT) {
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
			}
			if (action == GLFW_PRESS) {
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			if (action == GLFW_RELEASE) {
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
			}
			if (action == GLFW_PRESS) {
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
			}
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event((float)xpos, (float) ypos);
			data.EventCallback(event);
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)xoffset, (float) yoffset);
			data.EventCallback(event);
		});
	}

	unsigned int WindowsWindow::GetHeight() {
		return m_Data.Height;
	}

	unsigned int WindowsWindow::GetWidth() {
		return m_Data.Width;
	}

	void WindowsWindow::OnUpdate() {
		glfwPollEvents();
	}

	void WindowsWindow::IsMinimised() {
		int width = 0, height = 0;
		glfwGetFramebufferSize(m_Window, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(m_Window, &width, &height);
			glfwWaitEvents();
		}
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() {
		return m_Data.VSync;
	}

	void WindowsWindow::Shutdown() {
		CL_CORE_INFO("Destroying Window");
		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}
}
#pragma once
#include "Crystal/Core/Window.h"
#include "Crystal/GraphicsContext.h"

#include <GLFW/glfw3.h>
namespace Crystal {

	class CRYSTAL_API WindowsWindow : public Window {
	public:
		WindowsWindow(const WindowProperties& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;
		virtual unsigned int GetWidth() override;
		virtual unsigned int GetHeight() override;

		virtual void SetEventCallBack(const EventCallBackFn callback) override { m_Data.EventCallback = callback; };
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() override;
		virtual void* GetNativeWindow() const { return m_Window; }
	private:
		virtual void Init(const WindowProperties& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_GraphicsContext;
		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			EventCallBackFn EventCallback;
		};

		WindowData m_Data;
		
	};
}


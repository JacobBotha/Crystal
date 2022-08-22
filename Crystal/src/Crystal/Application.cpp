#include "clpch.h"

#include "Application.h"
#include "Crystal/Events/Event.h"
#include "Crystal/Events/ApplicationEvent.h"
#include "Crystal/Log.h"
//#include "Platform/Windows/WindowsWindow.h"

namespace Crystal {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application() {
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallBack(BIND_EVENT_FN(OnEvent));
		m_Running = true;
	}

	Application::~Application() {}

	void Application::OnEvent(Event& e) {
		//Check if window was closed
		if (e.GetEventType() == EventType::WindowClose) {
			m_Running = false;
		}

		//Log Event
		CL_TRACE(e);
	}

	void Application::run() {
		WindowResizeEvent e(1200, 950);
		CL_TRACE(e);
		while (m_Running) {
			m_Window->OnUpdate();
		}
	}
}
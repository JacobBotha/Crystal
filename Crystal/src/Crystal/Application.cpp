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
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		//ApplyEvent to all layers in the layer stack
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}

		//Log Event
		CL_TRACE(e);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}
	
	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}

	void Application::run() {
		WindowResizeEvent e(1200, 950);
		CL_TRACE(e);
		while (m_Running) {
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;

		return true;
	}
}
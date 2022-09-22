#include "clpch.h"

#include "Application.h"
#include "Crystal/Events/Event.h"
#include "Crystal/Events/ApplicationEvent.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		CL_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallBack(BIND_EVENT_FN(OnEvent));
		m_Running = true;

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application() {
		for (Layer* layer : m_LayerStack)
			layer->OnDetach();
		Renderer::Shutdown();
	}

	void Application::OnEvent(Event& e) {
		//Check if window was closed
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		//ApplyEvent to all layers in the layer stack
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	
	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Run() {
		while (m_Running) {
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			Renderer::DrawFrame();
			Renderer::PresentFrame();
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;

		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {
		//Check if window is minimized
		m_Window->IsMinimised();
		
		Renderer::WindowResized(e);

		return true;
	}
}
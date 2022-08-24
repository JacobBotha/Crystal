#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Crystal/Events/ApplicationEvent.h"

namespace Crystal {
	class CRYSTAL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void run();

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		LayerStack m_LayerStack;
		std::unique_ptr<Window> m_Window;
		bool m_Running;
	};

	//To be defined in client
	Application* CreateApplication();
}

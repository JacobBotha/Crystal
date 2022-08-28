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
		void Run();

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		LayerStack m_LayerStack;
		std::unique_ptr<Window> m_Window;
		bool m_Running;

		static Application* s_Instance;
	};

	//To be defined in client
	Application* CreateApplication();
}

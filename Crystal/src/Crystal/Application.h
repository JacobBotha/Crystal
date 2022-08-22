#pragma once

#include "Core.h"
#include "Window.h"

namespace Crystal {
	class CRYSTAL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void OnEvent(Event& e);
		void run();

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running;
	};

	//To be defined in client
	Application* CreateApplication();
}
